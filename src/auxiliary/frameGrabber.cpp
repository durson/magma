/*
Magma - Abstraction layer over Khronos Vulkan API.
Copyright (C) 2018-2024 Victor Coda.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "pch.h"
#pragma hdrstop
#include "frameGrabber.h"
#include "../objects/device.h"
#include "../objects/physicalDevice.h"
#include "../objects/image2D.h"
#include "../objects/commandBuffer.h"
#include "../objects/queue.h"
#include "../objects/fence.h"
#include "../misc/deviceFeatures.h"
#include "../misc/mapImage.h"
#include "../barriers/imageMemoryBarrier.h"

namespace magma::aux
{
FrameGrabber::FrameGrabber(std::shared_ptr<Device> device,
    std::shared_ptr<Allocator> allocator /* nullptr */):
    device(std::move(device)),
    allocator(std::move(allocator))
{}

void FrameGrabber::captureFrame(lent_ptr<Image2D> srcImage, lent_ptr<CommandBuffer> cmdBuffer)
{   // Allocate linear tiled image to copy pixels to
    dstImage = std::make_unique<LinearTiledImage2D>(device, VK_FORMAT_R8G8B8A8_UNORM,
        srcImage->getExtent2D(), allocator);
    const std::unique_ptr<DeviceFeatures>& features = device->getPhysicalDevice()->features();
    const bool srcBlit = features->supportsFormatFeatures(srcImage->getFormat(), VK_FORMAT_FEATURE_BLIT_SRC_BIT).optimal;
    const bool dstBlit = features->supportsFormatFeatures(dstImage->getFormat(), VK_FORMAT_FEATURE_BLIT_DST_BIT).linear;
    const VkImageLayout oldLayout = srcImage->getLayout(0);
    const VkFormat format = srcImage->getFormat();
    MAGMA_ASSERT(cmdBuffer->getState() == CommandBuffer::State::Recording);
    // Transition of destination image to transfer dest optimal layout
    ImageMemoryBarrier dstImageBarrier(dstImage.get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, dstImageBarrier);
    // Transition of source image to transfer source optimal layout
    ImageMemoryBarrier srcImageBarrier(srcImage.get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, srcImageBarrier);
    if (srcBlit && dstBlit)
    {   // Use image blitting, format swizzle will be performed automatically
        VkOffset3D blitSize;
        blitSize.x = static_cast<int32_t>(srcImage->getExtent().width);
        blitSize.y = static_cast<int32_t>(srcImage->getExtent().height);
        blitSize.z = 1;
        VkImageBlit blitRegion;
        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.mipLevel = 0;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcOffsets[0] = VkOffset3D{0, 0, 0};
        blitRegion.srcOffsets[1] = blitSize;
        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.mipLevel = 0;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstOffsets[0] = VkOffset3D{0, 0, 0};
        blitRegion.dstOffsets[1] = blitSize;
        cmdBuffer->blitImage(std::move(srcImage), dstImage, blitRegion, VK_FILTER_NEAREST);
    }
    else
    {   // Use image copy, format swizzle will be performed later manually
        VkImageCopy copyRegion;
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.srcOffset = VkOffset3D{0, 0, 0};
        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.dstOffset = VkOffset3D{0, 0, 0};
        copyRegion.extent = dstImage->calculateMipExtent(0);
        cmdBuffer->copyImage(std::move(srcImage), dstImage, copyRegion);
    }
    // Transition of destination image to general layout, which is the required layout for mapping
    dstImageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, dstImageBarrier);
    // Transition of swapchain image back to old layout
    srcImageBarrier.newLayout = oldLayout;
    cmdBuffer->pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, srcImageBarrier);
    // Do we need to handle swizzling?
    swizzleBgra = false;
    if (!srcBlit || !dstBlit)
    {
        if ((VK_FORMAT_B8G8R8A8_UNORM == format) || (VK_FORMAT_B8G8R8A8_SRGB == format))
            swizzleBgra = true;
    }
}

void FrameGrabber::readPixels(std::function<void(uint32_t col, uint32_t row, uint32_t rgba)> eachPixel) const
{   // Get image row pitch
    const VkSubresourceLayout subresourceLayout = dstImage->getSubresourceLayout(0);
    const VkDeviceSize rowPitch = subresourceLayout.rowPitch;
    const uint32_t width = dstImage->getWidth();
    const uint32_t height = dstImage->getHeight();
    mapImageRange<uint8_t>(dstImage, subresourceLayout.offset, VK_WHOLE_SIZE,
        [this, width, height, rowPitch, &eachPixel](const uint8_t *data)
        {
            for (uint32_t y = 0; y < height; ++y)
            {
                const uint32_t *row = (const uint32_t *)data;
                for (uint32_t x = 0; x < width; ++x)
                {
                    uint32_t abgr;
                    if (!swizzleBgra)
                        abgr = row[x];
                    else
                    {   // Swizzle BGRA to RGBA
                        const uint32_t argb = row[x];
                        const uint32_t r = (argb >> 16) & 0xFF;
                        const uint32_t g = (argb >> 8) & 0xFF;
                        const uint32_t b = argb & 0xFF;
                        abgr = (argb & 0xFF000000) | (b << 16) | (g << 8) | r;
                    }
                    eachPixel(x, y, abgr);
                }
                data += rowPitch;
            }
        }
    );
}

void FrameGrabber::readPixels(std::function<void(uint32_t row, const std::vector<uint32_t>& rowPixels)> eachRow) const
{   // Get image row pitch
    const VkSubresourceLayout subresourceLayout = dstImage->getSubresourceLayout(0);
    const VkDeviceSize rowPitch = subresourceLayout.rowPitch;
    const uint32_t width = dstImage->getWidth();
    const uint32_t height = dstImage->getHeight();
    mapImageRange<uint8_t>(dstImage, subresourceLayout.offset, VK_WHOLE_SIZE,
        [this, width, height, rowPitch, &eachRow](const uint8_t *data)
        {
            std::vector<uint32_t> rowPixels(width);
            for (uint32_t y = 0; y < height; ++y)
            {
                const uint32_t *row = (const uint32_t *)data;
                if (!swizzleBgra)
                    memcpy(rowPixels.data(), row, sizeof(uint32_t) * width);
                else for (uint32_t x = 0; x < width; ++x)
                {   // Swizzle BGRA to RGBA
                    const uint32_t argb = row[x];
                    const uint32_t r = (argb >> 16) & 0xFF;
                    const uint32_t g = (argb >> 8) & 0xFF;
                    const uint32_t b = argb & 0xFF;
                    rowPixels[x] = (argb & 0xFF000000) | (b << 16) | (g << 8) | r;
                }
                eachRow(y, rowPixels);
                data += rowPitch;
            }
        }
    );
}

VkExtent2D FrameGrabber::getImageExtent() const
{
    if (dstImage)
        return VkExtent2D{dstImage->getWidth(), dstImage->getHeight()};
    return VkExtent2D{0, 0};
}
} // namespace magma::aux
