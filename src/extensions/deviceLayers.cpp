/*
Magma - Abstraction layer over Khronos Vulkan API.
Copyright (C) 2018-2025 Victor Coda.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY), without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "pch.h"
#pragma hdrstop
#include "deviceLayers.h"
#include "../objects/physicalDevice.h"

namespace magma
{
DeviceLayers::DeviceLayers(const std::shared_ptr<PhysicalDevice>& device):
    ExtensionRegistry<VkLayerProperties>(device->enumerateLayers()),

    MAGMA_CHECK_LAYER(AMD_switchable_graphics),

    MAGMA_CHECK_LAYER(KHRONOS_validation)
{}
} // namespace magma
