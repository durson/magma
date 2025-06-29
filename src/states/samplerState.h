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
    /* 1. Nearest filtering is a sampling of nearest texel.
       2. Bilinear filtering is an interpolation of four texels in two directions.
       3. Trilinear filtering also performs linear interpolation between mipmaps.
       4. Anisotropic filtering performs long and narrow projection of the pixel
          filter footprint into texture space.
       5. Cubic filtering is an hermite interpolation of four coefficients to
          reconstruct a signal. */

    enum class MagnificationFilter : uint8_t
    {
        Nearest, Bilinear, Cubic, Unknown
    };

    enum class MinificationFilter : uint8_t
    {
        Nearest, Bilinear, Trilinear, Anisotropic, Mixed
    };

    /* Sampler state defines texture filtration parameters
       during texture sampling in the fragment shader. */

    struct SamplerState : VkSamplerCreateInfo
    {
    public:
        constexpr SamplerState(VkFilter magFilter,
            VkFilter minFilter,
            VkSamplerMipmapMode mipmapMode,
            VkSamplerAddressMode addressMode) noexcept;
        constexpr SamplerState(const SamplerState&) noexcept;
        constexpr bool chained() const noexcept { return pNext != nullptr; }
        constexpr MagnificationFilter getMagFilter() const noexcept;
        constexpr MinificationFilter getMinFilter() const noexcept;
        constexpr hash_t hash() const noexcept;
        hash_t chainedHash() const noexcept;
        constexpr bool operator==(const SamplerState&) const noexcept;
    };

    /* Anisotropic sampler state takes into account long and narrow
       (i.e. anisotropic) projection of the pixel filter footprint
       into texture space. */

    struct AnisotropicSamplerState : SamplerState
    {
        constexpr AnisotropicSamplerState(VkSamplerAddressMode addressMode,
            float maxAnisotropy) noexcept;
    };
}

#include "samplerState.inl"

#define MAGMA_SAMPLER_STATE_PERMUTATIONS(Suffix, addressMode)\
    constexpr SamplerState magMinMipNearest##Suffix(VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magLinearMinMipNearest##Suffix(VK_FILTER_LINEAR, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magNearestMinLinearMipNearest##Suffix(VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magMinNearestMipLinear##Suffix(VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);\
    constexpr SamplerState magMinLinearMipNearest##Suffix(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magNearestMinMipLinear##Suffix(VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);\
    constexpr SamplerState magLinearMinNearestMipLinear##Suffix(VK_FILTER_LINEAR, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);\
    constexpr SamplerState magMinMipLinear##Suffix(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X1(addressMode, 1.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X2(addressMode, 2.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X3(addressMode, 3.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X4(addressMode, 4.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X5(addressMode, 5.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X6(addressMode, 6.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X7(addressMode, 7.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X8(addressMode, 8.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X9(addressMode, 9.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X10(addressMode, 10.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X11(addressMode, 11.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X12(addressMode, 12.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X13(addressMode, 13.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X14(addressMode, 14.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X15(addressMode, 15.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##X16(addressMode, 16.f);\
    constexpr AnisotropicSamplerState magMinLinearMipAnisotropic##Suffix##Max(addressMode, std::numeric_limits<float>::max());

#ifdef VK_EXT_filter_cubic
#define MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(Suffix, addressMode)\
    constexpr SamplerState magCubicMinMipNearest##Suffix(VK_FILTER_CUBIC_EXT, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magCubicMinLinearMipNearest##Suffix(VK_FILTER_CUBIC_EXT, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, addressMode);\
    constexpr SamplerState magCubicMinMipLinear##Suffix(VK_FILTER_CUBIC_EXT, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);\
    constexpr SamplerState magMinCubicMipLinear##Suffix(VK_FILTER_CUBIC_EXT, VK_FILTER_CUBIC_EXT, VK_SAMPLER_MIPMAP_MODE_LINEAR, addressMode);
#endif // VK_EXT_filter_cubic

namespace magma::sampler
{
    MAGMA_SAMPLER_STATE_PERMUTATIONS(Repeat, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    MAGMA_SAMPLER_STATE_PERMUTATIONS(MirroredRepeat, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
    MAGMA_SAMPLER_STATE_PERMUTATIONS(ClampToEdge, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
    MAGMA_SAMPLER_STATE_PERMUTATIONS(ClampToBorder, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
#ifdef VK_KHR_sampler_mirror_clamp_to_edge
    // Original extension did not use an author suffix on the enum VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
    MAGMA_SAMPLER_STATE_PERMUTATIONS(MirrorClampToEdge, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE)
#endif

#ifdef VK_EXT_filter_cubic
    MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(Repeat, VK_SAMPLER_ADDRESS_MODE_REPEAT)
    MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(MirroredRepeat, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
    MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(ClampToEdge, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
    MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(ClampToBorder, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
#ifdef VK_KHR_sampler_mirror_clamp_to_edge
    // Original extension did not use an author suffix on the enum VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
    MAGMA_CUBIC_SAMPLER_STATE_PERMUTATIONS(MirrorClampToEdge, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE)
#endif
#endif // VK_EXT_filter_cubic
} // namespace magma::sampler
