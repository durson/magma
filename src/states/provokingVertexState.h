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

#ifdef VK_EXT_provoking_vertex
namespace magma
{
    /* Allows changing the provoking vertex convention between Vulkan's
       default convention (first vertex) and OpenGL's convention (last vertex). */

    struct ProvokingVertexRasterizationState : RasterizationState
    {
        constexpr ProvokingVertexRasterizationState(const RasterizationState& state,
            VkProvokingVertexModeEXT provokingVertexMode) noexcept;
        constexpr hash_t hash() const noexcept;
        constexpr bool operator==(const ProvokingVertexRasterizationState&) const noexcept;

        const VkPipelineRasterizationProvokingVertexStateCreateInfoEXT provokingVertexInfo;
    };
} // namespace magma

#include "provokingVertexState.inl"

#define MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(polygonMode, ProvokingMode, provokingVertexMode)\
    constexpr ProvokingVertexRasterizationState polygonMode##CullNoneCcw##ProvokingMode(polygonMode##CullNoneCcw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullFrontCcw##ProvokingMode(polygonMode##CullFrontCcw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullBackCcw##ProvokingMode(polygonMode##CullBackCcw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullFrontAndBackCcw##ProvokingMode(polygonMode##CullFrontAndBackCcw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullNoneCw##ProvokingMode(polygonMode##CullNoneCw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullFrontCw##ProvokingMode(polygonMode##CullFrontCw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullBackCw##ProvokingMode(polygonMode##CullBackCw, provokingVertexMode);\
    constexpr ProvokingVertexRasterizationState polygonMode##CullFrontAndBackCw##ProvokingMode(polygonMode##CullFrontAndBackCw, provokingVertexMode);

namespace magma::renderstate
{
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(fill, ProvokingFirst, VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(line, ProvokingFirst, VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(point, ProvokingFirst, VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(fill, ProvokingLast, VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(line, ProvokingLast, VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(point, ProvokingLast, VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT)
#ifdef VK_NV_fill_rectangle
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(fillRectangle, ProvokingFirst, VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT)
    MAGMA_PROVOKING_VERTEX_RASTERIZATION_STATE_PERMUTATIONS(fillRectangle, ProvokingLast, VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT)
#endif
} // namespace magma::renderstate
#endif // VK_EXT_provoking_vertex
