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
#include "colorBlendAttachmentState.h"

namespace magma
{
    /* Blending combines the incoming source fragment's
       R, G, B, and A values with the destination R, G, B,
       and A values of each sample stored in the framebuffer
       at the fragment's location. Blending is performed for
       each pixel sample, rather than just once for each fragment. */

    struct ColorBlendState : VkPipelineColorBlendStateCreateInfo
    {
        constexpr ColorBlendState() noexcept;
        constexpr ColorBlendState(const ColorBlendAttachmentState& attachment,
            bool logicOpEnable = false,
            VkLogicOp logicOp = VK_LOGIC_OP_CLEAR,
            VkPipelineColorBlendStateCreateFlags flags = 0,
            const std::initializer_list<float>& blendConstants = {1.f, 1.f, 1.f, 1.f}) noexcept;
        constexpr ColorBlendState(const ColorBlendState&) noexcept;
        constexpr hash_t hash() const noexcept;
        constexpr bool operator==(const ColorBlendState&) const noexcept;
        friend std::ostream& operator<<(std::ostream&, const ColorBlendState&);
    };
} // namespace magma

#include "colorBlendState.inl"

#define MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(name, state)\
    constexpr ColorBlendState name##R(blendstate::state##R);\
    constexpr ColorBlendState name##G(blendstate::state##G);\
    constexpr ColorBlendState name##B(blendstate::state##B);\
    constexpr ColorBlendState name##A(blendstate::state##A);\
    constexpr ColorBlendState name##Rg(blendstate::state##Rg);\
    constexpr ColorBlendState name##Rb(blendstate::state##Rb);\
    constexpr ColorBlendState name##Ra(blendstate::state##Ra);\
    constexpr ColorBlendState name##Gb(blendstate::state##Gb);\
    constexpr ColorBlendState name##Ga(blendstate::state##Ga);\
    constexpr ColorBlendState name##Ba(blendstate::state##Ba);\
    constexpr ColorBlendState name##Rgb(blendstate::state##Rgb);\
    constexpr ColorBlendState name##Rga(blendstate::state##Rga);\
    constexpr ColorBlendState name##Rba(blendstate::state##Rba);\
    constexpr ColorBlendState name##Gba(blendstate::state##Gba);\
    constexpr ColorBlendState name##Rgba(blendstate::state##Rgba);

namespace magma::renderstate
{
    constexpr ColorBlendState dontWriteRgba(blendstate::writeNone);
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(dontBlend, write)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendAdd, add)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendSubtract, subtract)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendReverseSubtract, reverseSubtract)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendMin, min)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendMax, max)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendMultiply, multiply)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendNormal, normal)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendNormalPremultiplied, normalPremultiplied)
    MAGMA_COLOR_BLEND_STATE_PERMUTATIONS(blendScreen, screen)
} // namespace magma::renderstate
