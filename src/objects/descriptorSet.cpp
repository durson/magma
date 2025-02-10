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
#include <sstream>
#include "descriptorSet.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"
#include "device.h"
#include "../descriptors/descriptorSetTable.h"
#include "../shaders/shaderReflection.h"
#include "../shaders/shaderReflectionFactory.h"
#include "../helpers/enumerationCast.h"
#include "../helpers/streamInsertOperators.h"
#include "../exceptions/errorResult.h"

namespace magma
{
DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> descriptorPool_,
    DescriptorSetTable& setTable, VkShaderStageFlags stageFlags,
    std::shared_ptr<IAllocator> allocator /* nullptr */,
    VkDescriptorSetLayoutCreateFlags flags /* 0 */,
    lent_ptr<IShaderReflectionFactory> shaderReflectionFactory /* nullptr */,
    std::string_view shaderFileName /* empty */,
    uint32_t setIndex /* 0 */,
    const StructureChain& extendedInfo /* default */):
    NonDispatchable(VK_OBJECT_TYPE_DESCRIPTOR_SET, descriptorPool_->getDevice(), std::move(allocator)),
    setTable(setTable),
    descriptorPool(std::move(descriptorPool_))
{   // Check that all descriptors have unique layout bindings
    const DescriptorSetTableBindings& reflection = setTable.getReflection();
    std::vector<uint32_t> locations;
    for (auto const& descriptor: reflection)
        locations.push_back(descriptor.get().binding);
    std::sort(locations.begin(), locations.end());
    if (std::unique(locations.begin(), locations.end()) != locations.end())
        MAGMA_ERROR("elements of descriptor set layout should have unique binding locations");
    if (shaderReflectionFactory && !shaderFileName.empty())
    {   // Validate descriptors through shader reflection
        auto& shaderReflection = shaderReflectionFactory->getReflection(std::move(shaderFileName));
        validateReflection(shaderReflection, setIndex);
    }
    // Prepare list of native bindings
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto const& descriptor: reflection)
    {
        bindings.push_back(descriptor.get());
        // Set global stage flags if they have not been assigned for descriptor binding
        if (!bindings.back().stageFlags)
            bindings.back().stageFlags = stageFlags;
    }
    // Create descriptor set layout
    setLayout = std::make_unique<DescriptorSetLayout>(device, bindings, hostAllocator, flags);
    // Allocate descriptor set
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = extendedInfo.headNode();
    descriptorSetAllocateInfo.descriptorPool = *descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = setLayout->getHandleAddress();
    const VkResult result = vkAllocateDescriptorSets(getNativeDevice(), &descriptorSetAllocateInfo, &handle);
    MAGMA_HANDLE_RESULT(result, "failed to allocate descriptor set");
    if (dirty())
    {   // Once allocated, descriptor set can be updated
        update();
    }
}

DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> descriptorPool_,
    DescriptorSetTable& setTable, std::shared_ptr<IAllocator> allocator):
    NonDispatchable(VK_OBJECT_TYPE_DESCRIPTOR_SET, descriptorPool_->getDevice(), std::move(allocator)),
    setTable(setTable),
    descriptorPool(std::move(descriptorPool_))
{}

DescriptorSet::~DescriptorSet()
{
    bool canFreeDescriptorSet = descriptorPool->getFlags() & VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    if (canFreeDescriptorSet)
        vkFreeDescriptorSets(getNativeDevice(), *descriptorPool, 1, &handle);
}

std::size_t DescriptorSet::getDescriptorCount() const
{
    return setTable.getSize();
}

bool DescriptorSet::dirty() const
{
    return setTable.dirty();
}

void DescriptorSet::update()
{
    MAGMA_VLA(VkWriteDescriptorSet, descriptorWrites, setTable.getSize());
    const uint32_t descriptorWriteCount = writeDescriptors(descriptorWrites);
    if (descriptorWriteCount)
        device->updateDescriptorSets(descriptorWriteCount, descriptorWrites, 0, nullptr);
}

uint32_t DescriptorSet::writeDescriptors(VkWriteDescriptorSet *descriptorWrites) const
{
    MAGMA_ASSERT(dirty());
    uint32_t descriptorWriteCount = 0;
    for (auto const& descriptor: setTable.getReflection())
    {   // Write dirty descriptors
        const DescriptorSetLayoutBinding& binding = descriptor.get();
        if (binding.modified())
            binding.write(handle, descriptorWrites[descriptorWriteCount++]);
    }
    return descriptorWriteCount;
}

void DescriptorSet::validateReflection(const std::unique_ptr<const ShaderReflection>& shaderReflection, uint32_t setIndex) const
{
    std::vector<const SpvReflectDescriptorSet *> descriptorSets = shaderReflection->enumerateDescriptorSets();
    if (setIndex >= descriptorSets.size())
        MAGMA_ERROR("set index exceeds number of reflected descriptor sets");
    const SpvReflectDescriptorSet *descriptorSet = descriptorSets[setIndex];
    for (auto const& descriptor: setTable.getReflection())
    {
        const DescriptorSetLayoutBinding& binding = descriptor.get();
        if (!binding.resourceBinded())
            std::cout << "warning: binding #" << binding.binding << " has no binded resource" << std::endl;
        const SpvReflectDescriptorBinding *reflectedBinding = nullptr;
        for (uint32_t i = 0; i < descriptorSet->binding_count; ++i)
        {
            if (binding.binding == descriptorSet->bindings[i]->binding)
            {
                reflectedBinding = descriptorSet->bindings[i];
                break;
            }
        }
        if (!reflectedBinding)
        {
            std::cout << "warning: binding #" << binding.binding << " not found in the descriptor set # " << setIndex << std::endl;
            continue;
        }
        std::ostringstream out;
        const VkDescriptorType reflectedDescriptorType = helpers::spirvToDescriptorType(reflectedBinding->descriptor_type);
        if (binding.descriptorType != reflectedDescriptorType)
        {
            if ((VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC == binding.descriptorType) &&
                (SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER != reflectedBinding->descriptor_type))
            {   // Descriptor type is different
                out << "descriptor type mismatch:" << std::endl
                    << "binding #" << binding.binding << std::endl
                    << "expected: " << reflectedDescriptorType << std::endl
                    << "defined: " << binding.descriptorType;
            }
        }
        if (binding.descriptorCount != reflectedBinding->count)
        {   // Array size (or number of bytes) is different
            out << "descriptor count mismatch:" << std::endl
                << "binding #" << binding.binding << std::endl
                << "expected: " << reflectedBinding->count << ", defined: " << binding.descriptorCount;
        }
        switch (binding.descriptorType)
        {
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            {
                const SpvReflectImageTraits& imageTraits = reflectedBinding->image;
                const VkImageType imageType = helpers::spirvDimToImageType(imageTraits.dim);
                if (binding.getImageType() != VK_IMAGE_TYPE_MAX_ENUM &&
                    binding.getImageType() != imageType)
                {   // Type of assigned image is different
                    out << "descriptor image type mismatch:" << std::endl
                        << "binding #" << binding.binding << std::endl
                        << "expected: " << imageType << std::endl
                        << "assigned: " << binding.getImageType();
                }
            }
            break;
        default:
            break;
        }
        const std::string error = out.str();
        if (!error.empty())
            MAGMA_ERROR(error.c_str());
    }
}
} // namespace magma
