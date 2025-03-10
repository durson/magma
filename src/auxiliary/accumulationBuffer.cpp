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
#include "accumulationBuffer.h"
#include "imageDescriptorSet.h"
#include "fillRectangleVertexShader.h"
#include "../objects/device.h"
#include "../objects/physicalDevice.h"
#include "../objects/framebuffer.h"
#include "../objects/imageAttachment.h"
#include "../objects/imageView.h"
#include "../objects/renderPass.h"
#include "../objects/shaderModule.h"
#include "../objects/sampler.h"
#include "../objects/descriptorSet.h"
#include "../objects/pipelineLayout.h"
#include "../objects/pipelineLibrary.h"
#include "../objects/graphicsPipeline.h"
#include "../objects/commandBuffer.h"
#include "../shaders/shaderStages.h"
#include "../shaders/shaderReflection.h"
#include "../states/vertexInputStructure.h"
#include "../states/inputAssemblyState.h"
#include "../states/tesselationState.h"
#include "../states/viewportState.h"
#include "../states/rasterizationState.h"
#include "../states/multisampleState.h"
#include "../states/depthStencilState.h"
#include "../states/colorBlendState.h"
#include "../states/samplerState.h"
#include "../misc/format.h"
#include "../allocator/allocator.h"
#include "../exceptions/exception.h"

namespace magma::aux
{
AccumulationBuffer::AccumulationBuffer(std::shared_ptr<Device> device, VkFormat format, const VkExtent2D& extent,
    std::shared_ptr<ShaderModule> fragmentShader,
    std::shared_ptr<Allocator> allocator /* nullptr */,
    lent_ptr<PipelineCache> pipelineCache /* nullptr */):
    count(0),
    maxCount(std::numeric_limits<uint32_t>::max())
{
    auto& reflection = fragmentShader->getReflection();
    if (!reflection)
        MAGMA_ERROR("shader reflection not found");
    std::shared_ptr<IAllocator> hostAllocator = MAGMA_HOST_ALLOCATOR(allocator);
    const AttachmentDescription attachment(format, 1,
        op::store,
        op::dontCare,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    renderPass = std::make_unique<RenderPass>(device, attachment, hostAllocator);
    // Let it know what view format will be paired with the image
    Image::Initializer imageFormatList;
    imageFormatList.viewFormats.push_back(format);
    // Create high-precision color buffer
    constexpr bool sampled = true;
    std::unique_ptr<ColorAttachment> accumBuffer = std::make_unique<ColorAttachment>(
        device, format, extent, 1, 1, sampled, allocator, false, imageFormatList);
    bufferView = std::make_shared<UniqueImageView>(std::move(accumBuffer));
    framebuffer = std::make_unique<Framebuffer>(renderPass, bufferView, hostAllocator, 0);
    // Create descriptor set for fragment shader
    descriptorSet = std::make_unique<ImageDescriptorSet>(device, reflection, hostAllocator);
    nearestSampler = std::make_unique<Sampler>(device, sampler::magMinMipNearestClampToEdge, hostAllocator);
    // Setup shader stages
    FillRectangleVertexShader vertexShaderStage(device, hostAllocator);
    const RasterizationState rasterizationState = vertexShaderStage.getRasterizationState();
    std::vector<PipelineShaderStage> shaderStages;
    shaderStages.emplace_back(std::move(vertexShaderStage));
    shaderStages.emplace_back(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(fragmentShader), reflection->getEntryPointName(0));
    // Create blending pipeline
    const uint8_t componentCount = Format(format).componentCount();
    constexpr push::FragmentConstantRange<float> pushConstantRange;
    std::unique_ptr<PipelineLayout> pipelineLayout = std::make_unique<PipelineLayout>(
        descriptorSet->getDescriptorSet()->getLayout(), pushConstantRange, hostAllocator);
    blendPipeline = std::make_unique<GraphicsPipeline>(std::move(device),
        shaderStages,
        renderstate::nullVertexInput,
        renderstate::triangleList,
        TesselationState(),
        ViewportState(extent),
        rasterizationState,
        renderstate::dontMultisample,
        renderstate::depthAlwaysDontWrite,
        (1 == componentCount) ? renderstate::blendNormalR :
        (2 == componentCount) ? renderstate::blendNormalRg :
        (3 == componentCount) ? renderstate::blendNormalRgb :
        (4 == componentCount) ? renderstate::blendNormalRgba :
        renderstate::dontBlendRgba,
        std::vector<VkDynamicState>{},
        std::move(pipelineLayout),
        renderPass, 0,
        std::move(hostAllocator),
        std::move(pipelineCache),
        nullptr); // basePipeline
}

AccumulationBuffer::~AccumulationBuffer() {}

void AccumulationBuffer::accumulate(lent_ptr<CommandBuffer> cmdBuffer, lent_ptr<ImageView> imageView) noexcept
{
    MAGMA_ASSERT(cmdBuffer);
    MAGMA_ASSERT(imageView);
    if (count < maxCount)
    {
        descriptorSet->writeDescriptor(std::move(imageView), nearestSampler);
        cmdBuffer->beginRenderPass(renderPass, framebuffer);
        {   // Calculate blend weight
            const float weight = 1.f - count / (1.f + count);
            cmdBuffer->pushConstant(blendPipeline->getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, weight);
            // Do normal blending
            cmdBuffer->bindDescriptorSet(blendPipeline, 0, descriptorSet->getDescriptorSet());
            cmdBuffer->bindPipeline(blendPipeline);
            cmdBuffer->draw(3);
        }
        cmdBuffer->endRenderPass();
        ++count;
    }
}
} // namespace magma::aux
