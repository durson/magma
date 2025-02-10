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
#include "exception.h"

namespace magma::exception
{
#ifdef MAGMA_NO_EXCEPTIONS
static ExceptionHandler exceptionHandler =
    [](std::string_view message, const source_location&)
    {   // If no exception handler is provided, simply call abort()
        std::cerr << message << std::endl;
        abort();
    };

void setExceptionHandler(ExceptionHandler exceptionHandler_) noexcept
{
    exceptionHandler = std::move(exceptionHandler_);
}
#endif // MAGMA_NO_EXCEPTIONS

void handleException(std::string_view message, const source_location& location)
{
#ifdef MAGMA_NO_EXCEPTIONS
    exceptionHandler(std::move(message), location);
#else
    throw Exception(std::move(message), location);
#endif
}
} // namespace magma::exception
