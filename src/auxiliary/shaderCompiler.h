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
#include <shaderc/shaderc.h>

namespace magma
{
    class Device;
    class ShaderModule;

    namespace aux
    {
        /* An abstract interface for mapping an #include request
           to a result source string. */

        class IShaderInclude : public IClass
        {
        public:
            virtual void *resolve(shaderc_include_type includeType,
                const char *requestedSource,
                const char *requestingSource,
                std::size_t includeDepth,
                std::size_t& dataSize) noexcept = 0;
            virtual void release(void *data) noexcept = 0;
        };

        /* Auxiliary wrapper around https://github.com/google/shaderc
           to compile GLSL shaders in run-time. Can be useful in case
           if shader code is generated dynamically. */

        class ShaderCompiler : public IClass
        {
        public:
            ShaderCompiler(std::shared_ptr<Device> device,
                std::shared_ptr<IShaderInclude> handler);
            ~ShaderCompiler();
            void setOptimizationLevel(shaderc_optimization_level optimizationLevel) noexcept;
            void setGenerateDebugInfo(bool generateDebugInfo) noexcept;
            void setSuppressWarnings(bool suppressWarnings) noexcept;
            void setWarningsAsErrors(bool warningsAsErrors) noexcept;
            std::shared_ptr<ShaderModule> compileShader(std::string_view source,
                std::string_view entrypoint,
                shaderc_shader_kind shaderKind = shaderc_glsl_infer_from_source,
                const std::unordered_map<std::string_view, std::string_view>& macroDefinitions = {},
                std::string_view srcFileName = {});

        private:
            std::shared_ptr<Device> device;
            std::shared_ptr<IShaderInclude> includeHandler;
            shaderc_compiler_t compiler;
            shaderc_optimization_level optimizationLevel = shaderc_optimization_level_zero;
            bool generateDebugInfo = false;
            bool suppressWarnings = false;
            bool warningsAsErrors = false;
        };
    } // namespace aux
} // namespace magma

#include "shaderCompiler.inl"
