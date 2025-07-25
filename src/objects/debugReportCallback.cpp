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
#include <cstdarg>
#include "debugReportCallback.h"
#include "instance.h"
#include "../allocator/allocator.h"
#include "../misc/extension.h"
#include "../helpers/enumerationCast.h"
#include "../exceptions/errorResult.h"

namespace magma
{
#ifdef VK_EXT_debug_report
DebugReportCallback::DebugReportCallback(Instance *instance,
    PFN_vkDebugReportCallbackEXT userCallback,
    std::shared_ptr<IAllocator> allocator /* nullptr */,
    VkDebugReportFlagsEXT flags /* INFORMATION_BIT | WARNING_BIT_EXT | PERFORMANCE_WARNING_BIT_EXT | ERROR_BIT DEBUG_BIT */,
    void *userData /* nullptr */,
    const StructureChain& extendedInfo /* default */):
    NonDispatchable(VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT, std::move(allocator)),
    instance(instance)
{
    MAGMA_ASSERT(userCallback);
    MAGMA_INSTANCE_EXTENSION(vkCreateDebugReportCallbackEXT);
    if (vkCreateDebugReportCallbackEXT)
    {
        VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfo;
        debugReportCallbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debugReportCallbackInfo.pNext = extendedInfo.headNode();
        debugReportCallbackInfo.flags = flags;
        debugReportCallbackInfo.pfnCallback = userCallback;
        debugReportCallbackInfo.pUserData = userData;
        const VkResult result = vkCreateDebugReportCallbackEXT(*instance, &debugReportCallbackInfo, MAGMA_OPTIONAL(hostAllocator), &handle);
        MAGMA_HANDLE_RESULT(result, "failed to create debug report callback");
    }
}

DebugReportCallback::~DebugReportCallback()
{
    MAGMA_INSTANCE_EXTENSION(vkDestroyDebugReportCallbackEXT);
    if (vkDestroyDebugReportCallbackEXT)
        vkDestroyDebugReportCallbackEXT(*instance, handle, MAGMA_OPTIONAL(hostAllocator));
}

void DebugReportCallback::message(VkDebugReportFlagsEXT flags, VkObjectType objectType,
    uint64_t object, std::size_t location, int32_t messageCode, const char *layerPrefix, const char *message) const noexcept
{
    MAGMA_INSTANCE_EXTENSION(vkDebugReportMessageEXT);
    if (vkDebugReportMessageEXT)
    {
        VkDebugReportObjectTypeEXT debugObjectType = helpers::objectToDebugReportType(objectType);
        vkDebugReportMessageEXT(*instance, flags, debugObjectType, object, location, messageCode, layerPrefix, message);
    }
}

void DebugReportCallback::messageFormat(VkDebugReportFlagsEXT flags, VkObjectType objectType,
    uint64_t object, std::size_t location, int32_t messageCode, const char *layerPrefix, const char *format, ...) const noexcept
{
    char buffer[MaxStringLength];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MaxStringLength, format, args);
    va_end(args);
    message(flags, objectType, object, location, messageCode, layerPrefix, buffer);
}
#endif // VK_EXT_debug_report
} // namespace magma
