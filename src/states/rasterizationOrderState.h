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
#include "rasterizationState.h"

#ifdef VK_AMD_rasterization_order
namespace magma
{
    /* This extension enables applications to opt into a relaxed,
       implementation defined primitive rasterization order that
       may allow better parallel processing of primitives and thus
       enabling higher primitive throughput. */

    struct RasterizationOrderState : RasterizationState
    {
        constexpr RasterizationOrderState(const RasterizationState& state,
            VkRasterizationOrderAMD rasterizationOrder) noexcept;
        constexpr RasterizationOrderState(const RasterizationOrderState&) noexcept;
        constexpr hash_t hash() const noexcept;
        constexpr bool operator==(const RasterizationOrderState&) const noexcept;

        const VkPipelineRasterizationStateRasterizationOrderAMD rasterizationOrderInfo;
    };
} // namespace magma

#include "rasterizationOrderState.inl"

#define MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(polygonMode, Order, rasterizationOrder)\
    constexpr RasterizationOrderState polygonMode##CullNoneCcw##Order(polygonMode##CullNoneCcw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullFrontCcw##Order(polygonMode##CullFrontCcw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullBackCcw##Order(polygonMode##CullBackCcw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullFrontAndBackCcw##Order(polygonMode##CullFrontAndBackCcw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullNoneCw##Order(polygonMode##CullNoneCw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullFrontCw##Order(polygonMode##CullFrontCw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullBackCw##Order(polygonMode##CullBackCw, rasterizationOrder);\
    constexpr RasterizationOrderState polygonMode##CullFrontAndBackCw##Order(polygonMode##CullFrontAndBackCw, rasterizationOrder);

namespace magma::renderstate
{
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(fill, Strict, VK_RASTERIZATION_ORDER_STRICT_AMD)
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(line, Strict, VK_RASTERIZATION_ORDER_STRICT_AMD)
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(point, Strict, VK_RASTERIZATION_ORDER_STRICT_AMD)
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(fill, Relaxed, VK_RASTERIZATION_ORDER_RELAXED_AMD)
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(line, Relaxed, VK_RASTERIZATION_ORDER_RELAXED_AMD)
    MAGMA_RASTERIZATION_ORDER_STATE_PERMUTATIONS(point, Relaxed, VK_RASTERIZATION_ORDER_RELAXED_AMD)
} // namespace magma::renderstate
#endif // VK_AMD_rasterization_order
