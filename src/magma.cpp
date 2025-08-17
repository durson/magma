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
//#define MAGMA_VERIFY_CONSTEXPR
//#define MAGMA_VERIFY_DESCRIPTORS
#if defined(MAGMA_VERIFY_CONSTEXPR) || defined(MAGMA_VERIFY_DESCRIPTORS)
#include "../magma.h"
#endif

namespace magma
{
#ifdef MAGMA_VERIFY_CONSTEXPR
    // Format
    constexpr Format rgbaUnormFormat(VK_FORMAT_R8G8B8A8_UNORM);
    constexpr const char *rgbaUnormName = rgbaUnormFormat.name();
    static_assert(rgbaUnormFormat.unorm(), "expected unorm format");
    static_assert(rgbaUnormFormat.alpha(), "expected alpha channel");
    static_assert(!rgbaUnormFormat.bc(), "expected uncompressed format");
    // Attachment description
    constexpr AttachmentDescription rgbaUnormAttachment(VK_FORMAT_R8G8B8A8_UNORM, 4, op::load, op::store,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    constexpr hash_t attachmentHash = rgbaUnormAttachment.hash();
    // Subpass dependency
    constexpr SubpassExternalDependency subpassDependency(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    constexpr hash_t subpassDependencyHash = subpassDependency.hash();
    // Input assembly state
    constexpr hash_t triangleListHash = renderstate::triangleList.hash();
    // Tesselation state
    constexpr TesselationState eightControlPointsPatch(8);
    constexpr hash_t eightControlPointsPatchHash = eightControlPointsPatch.hash();
    // Rasterization state
    constexpr hash_t fillCullBackCcwHash = renderstate::fillCullBackCcw.hash();
    // Depth/stencil states
    constexpr hash_t depthLessOrEqualHash = renderstate::depthLessOrEqual.hash();
    constexpr hash_t stencilOneDepthPassHash = renderstate::stencilOneDepthPass.hash();
    // Blend states
    constexpr hash_t writeNoneHash = blendstate::writeNone.hash();
    constexpr hash_t dontWriteRgbaHash = renderstate::dontWriteRgba.hash();
    // Sampler state
    constexpr hash_t magMinMipLinearRepeatHash = sampler::magMinMipLinearRepeat.hash();
    // Depth comparison sampler state
    constexpr hash_t magMinNearestLessOrEqualHash = sampler::magMinNearestLessOrEqual.hash();
    // Multisample state
    constexpr hash_t msaa4xHash = renderstate::msaa4x.hash();
    // Extended states
#ifdef VK_AMD_rasterization_order
    constexpr hash_t fillCullNoneCcwStrictHash = renderstate::fillCullNoneCcwStrict.hash();
#endif
#ifdef VK_EXT_conservative_rasterization
    constexpr hash_t fillCullNoneCcwOverestimateHash = renderstate::fillCullNoneCcwOverestimate.hash();
#endif
#ifdef VK_EXT_line_rasterization
    constexpr hash_t stippledLineBresenhamPatternA2xHash = renderstate::stippledLineBresenhamPatternA2x.hash();
#endif
#ifdef VK_EXT_provoking_vertex
    constexpr hash_t fillCullBackCcwProvokingFirstHash = renderstate::fillCullBackCcwProvokingFirst.hash();
#endif
#ifdef VK_EXT_transform_feedback
    constexpr hash_t fillCullNoneCcwStream1Hash = renderstate::fillCullNoneCcwStream1.hash();
#endif
#ifdef VK_EXT_blend_operation_advanced
    constexpr hash_t blendOverlayRgbaHash = blendstate::advanced::blendOverlayRgba.hash();
#endif
#ifdef VK_EXT_sampler_filter_minmax
    constexpr hash_t magMinMipLinearRepeatWeightedAverageHash = sampler::magMinMipLinearRepeatWeightedAverage.hash();
#endif
#ifdef VK_KHR_sampler_ycbcr_conversion
    constexpr hash_t rgbIdentityFullNearestHash = sampler::rgbIdentityFullNearest.hash();
#endif
    // Custom border color
    constexpr VkClearColorValue redColor{1.f, 0.f, 0.f, 1.f};
    constexpr BorderColor customBorderColor(redColor);
    constexpr hash_t customBorderColorHash = customBorderColor.hash();
    // Push constant
    struct PushConstants
    {
        float m[16];
    };
    constexpr push::VertexFragmentConstantRange<PushConstants> pushConstant;
    constexpr hash_t pushConstantHash = pushConstant.hash();

    // Magnification filter
    static_assert(sampler::magMinMipNearestRepeat.getMagFilter() == MagnificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magLinearMinMipNearestRepeat.getMagFilter() == MagnificationFilter::Bilinear, "expected bilinear filter");
    static_assert(sampler::magNearestMinLinearMipNearestRepeat.getMagFilter() == MagnificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magMinNearestMipLinearRepeat.getMagFilter() == MagnificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magMinLinearMipNearestRepeat.getMagFilter() == MagnificationFilter::Bilinear, "expected bilinear filter");
    static_assert(sampler::magNearestMinMipLinearRepeat.getMagFilter() == MagnificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magLinearMinNearestMipLinearRepeat.getMagFilter() == MagnificationFilter::Bilinear, "expected bilinear filter");
    static_assert(sampler::magMinMipLinearRepeat.getMagFilter() == MagnificationFilter::Bilinear, "expected bilinear filter");

    // Minification filter
    static_assert(sampler::magMinMipNearestRepeat.getMinFilter() == MinificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magLinearMinMipNearestRepeat.getMinFilter() == MinificationFilter::Nearest, "expected nearest filter");
    static_assert(sampler::magNearestMinLinearMipNearestRepeat.getMinFilter() == MinificationFilter::Bilinear, "expected bilinear filter");
    static_assert(sampler::magMinNearestMipLinearRepeat.getMinFilter() == MinificationFilter::Mixed, "expected mixed filter");
    static_assert(sampler::magMinLinearMipNearestRepeat.getMinFilter() == MinificationFilter::Bilinear, "expected bilinear filter");
    static_assert(sampler::magNearestMinMipLinearRepeat.getMinFilter() == MinificationFilter::Trilinear, "expected trilinear filter");
    static_assert(sampler::magLinearMinNearestMipLinearRepeat.getMinFilter() == MinificationFilter::Mixed, "exepected mixed filter");
    static_assert(sampler::magMinMipLinearRepeat.getMinFilter() == MinificationFilter::Trilinear, "expected trilinear filter");
    static_assert(sampler::magMinLinearMipAnisotropicRepeat1x.getMinFilter() == MinificationFilter::Anisotropic, "expected anisotropic filter");

    // Numeric formats
    static_assert(UByteNorm::numericFormat() == NumericFormat::UNorm, "expected UNORM format");
    static_assert(ByteNorm::numericFormat() == NumericFormat::SNorm, "expected SNORM format");
    static_assert(UByte::numericFormat() == NumericFormat::UInt, "expected UINT format");
    static_assert(Byte::numericFormat() == NumericFormat::SInt, "expected SINT format");
    static_assert(UShortNorm::numericFormat() == NumericFormat::UNorm, "expected UNORM format");
    static_assert(ShortNorm::numericFormat() == NumericFormat::SNorm, "expected SNORM format");
    static_assert(UShort::numericFormat() == NumericFormat::UInt, "expected UINT format");
    static_assert(Short::numericFormat() == NumericFormat::SInt, "expected SINT format");
    static_assert(Half::numericFormat() == NumericFormat::SFloat, "expected SFLOAT format");
    static_assert(UInt::numericFormat() == NumericFormat::UInt, "expected UINT format");
    static_assert(Int::numericFormat() == NumericFormat::SInt, "expected SINT format");
    static_assert(Float::numericFormat() == NumericFormat::SFloat, "expected SFLOAT format");
    static_assert(LargeUInt::numericFormat() == NumericFormat::UInt, "expected UINT format");
    static_assert(LargeInt::numericFormat() == NumericFormat::SInt, "expected SINT format");
    static_assert(Double::numericFormat() == NumericFormat::SFloat, "expected SFLOAT format");
#endif // MAGMA_VERIFY_CONSTEXPR

#ifdef MAGMA_VERIFY_DESCRIPTORS
    struct BufferSetTable
    {
        descriptor::UniformTexelBuffer uniformTexelBuffer = 0;
        descriptor::StorageTexelBuffer storageTexelBuffer = 1;
        descriptor::UniformBuffer uniformBuffer = 2;
        descriptor::StorageBuffer storageBuffer = 3;
        descriptor::DynamicUniformBuffer dynamicUniformBuffer = 4;
        descriptor::DynamicStorageBuffer dynamicStorageBuffer = 5;
        descriptor::UniformTexelBufferArray<4> uniformTexelBufferArray = 6;
        descriptor::StorageTexelBufferArray<4> storageTexelBufferArray = 7;
        descriptor::UniformBufferArray<4> uniformBufferArray = 8;
        descriptor::StorageBufferArray<4> storageBufferArray = 9;
        descriptor::DynamicUniformBufferArray<4> dynamicUniformBufferArray = 10;
        descriptor::DynamicStorageBufferArray<4> dynamicStorageBufferArray = 11;
    };

    struct ImageSetTable
    {
        descriptor::Sampler sampler = 0;
        descriptor::CombinedImageSampler combinedImageSampler = 1;
        descriptor::CombinedImageImmutableSampler combinedImageImmutableSampler = 2;
        descriptor::SampledImage sampledImage = 3;
        descriptor::StorageImage storageImage = 4;
        descriptor::InputAttachment inputAttachment = 5;
        descriptor::SamplerArray<4> samplerArray = 6;
        descriptor::CombinedImageSamplerArray<4> combinedImageSamplerArray = 7;
        descriptor::CombinedImageImmutableSamplerArray<4> combinedImageImmutableSamplerArray = 8;
        descriptor::SampledImageArray<4> sampledImageArray = 9;
        descriptor::StorageImageArray<4> storageImageArray = 10;
        descriptor::InputAttachmentArray<4> inputAttachmentArray = 11;
    };

    struct ExtSetTable
    {
    #ifdef VK_EXT_inline_uniform_block
        struct Uniforms
        {
            float data[4];
        };
        descriptor::InlineUniformBlock<Uniforms> inlineUniformBlock = 0;
    #endif // VK_EXT_inline_uniform_block
    #ifdef VK_KHR_acceleration_structure
        descriptor::AccelerationStructure accelerationStructure = 1;
    #endif // VK_KHR_acceleration_structure
    #ifdef VK_EXT_descriptor_indexing
        descriptor::CombinedImageSamplerVariableArray combinedImageSamplerVariableArray = 2;
        descriptor::SampledImageVariableArray sampledImageVariableArray = 3;
    #endif // VK_EXT_descriptor_indexing
    };
#endif // MAGMA_VERIFY_DESCRIPTORS
} // namespace magma
