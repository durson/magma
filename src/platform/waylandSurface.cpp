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
#include "waylandSurface.h"
#include "../objects/instance.h"
#include "../objects/device.h"
#include "../allocator/allocator.h"
#include "../misc/extension.h"
#include "../exceptions/errorResult.h"

namespace magma
{
#ifdef VK_KHR_wayland_surface
WaylandSurface::WaylandSurface(Instance *instance,
    wl_display *display, wl_surface *surface,
    std::shared_ptr<IAllocator> allocator /* nullptr */,
    VkWaylandSurfaceCreateFlagsKHR flags /* 0 */):
    Surface(instance, std::move(allocator)),
    display(display),
    surface(surface)
{
    VkWaylandSurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.flags = flags;
    surfaceInfo.display = display;
    surfaceInfo.surface = surface;
    const VkResult result = vkCreateWaylandSurfaceKHR(*instance, &surfaceInfo,
        MAGMA_OPTIONAL(hostAllocator), &handle);
    MAGMA_HANDLE_RESULT(result, "failed to create Wayland surface");
}
#endif // VK_KHR_wayland_surface
} // namespace magma
