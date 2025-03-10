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
#pragma once

namespace magma
{
    class Device;
    class Framebuffer;
    class ColorAttachment;
    class ImageView;
    class RenderPass;
    class ShaderModule;
    class Specialization;
    class Sampler;
    class GraphicsPipeline;
    class PipelineCache;
    class CommandBuffer;
    class Allocator;

    struct Descriptor;

    namespace aux
    {
        class ImageDescriptorSet;

        /* Accumulation buffer is dedicated for integrating multiple
           images. In order to maintain accuracy over a number of
           blending operations, the accumulation buffer should have
           a higher number of bits per component than a typical color
           buffer, so use R16_UNORM or R16_SFLOAT format. */

        class AccumulationBuffer : public IClass
        {
        public:
            explicit AccumulationBuffer(std::shared_ptr<Device> device,
                VkFormat format,
                const VkExtent2D& extent,
                std::shared_ptr<ShaderModule> fragmentShader,
                std::shared_ptr<Allocator> allocator = nullptr,
                lent_ptr<PipelineCache> pipelineCache = nullptr);
            ~AccumulationBuffer();
            const std::shared_ptr<ImageView>& getBufferView() const noexcept { return bufferView; }
            uint32_t getCount() const noexcept { return count; }
            void setAccumLimit(uint32_t count) noexcept { maxCount = count; }
            void accumulate(lent_ptr<CommandBuffer> cmdBuffer,
                lent_ptr<ImageView> imageView) noexcept;
            void reset() noexcept { count = 0; }

        private:
            std::shared_ptr<ImageView> bufferView;
            std::unique_ptr<RenderPass> renderPass;
            std::unique_ptr<Framebuffer> framebuffer;
            std::unique_ptr<ImageDescriptorSet> descriptorSet;
            std::unique_ptr<Sampler> nearestSampler;
            std::unique_ptr<GraphicsPipeline> blendPipeline;
            uint32_t count;
            uint32_t maxCount;
        };
    } // namespace aux
} // namespace magma

