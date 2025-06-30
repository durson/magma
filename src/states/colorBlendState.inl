namespace magma
{
constexpr ColorBlendState::ColorBlendState() noexcept:
    VkPipelineColorBlendStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr, // pNext
        0, // flags
        VK_FALSE, // logicOpEnable
        VK_LOGIC_OP_CLEAR, // logicOp
        0, // attachmentCount
        nullptr, // pAttachments
        {0.f, 0.f, 0.f, 0.f} // blendConstants
    }
{}

constexpr ColorBlendState::ColorBlendState(const ColorBlendAttachmentState& attachment,
    bool logicOpEnable /* false */,
    const VkLogicOp logicOp /* VK_LOGIC_OP_CLEAR */,
    VkPipelineColorBlendStateCreateFlags flags /* 0 */,
    const std::initializer_list<float>& blendConstants /* {1, 1, 1, 1} */) noexcept:
    VkPipelineColorBlendStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        nullptr, // pNext
        flags,
        MAGMA_BOOLEAN(logicOpEnable),
        logicOp,
        1, // attachmentCount
        &attachment,
        {
            blendConstants.begin()[0],
            blendConstants.begin()[1],
            blendConstants.begin()[2],
            blendConstants.begin()[3]
        }
    }
{}

constexpr ColorBlendState::ColorBlendState(const ColorBlendState& other) noexcept:
    VkPipelineColorBlendStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        other.pNext,
        other.flags,
        other.logicOpEnable,
        other.logicOp,
        other.attachmentCount,
        other.pAttachments,
        {
            other.blendConstants[0],
            other.blendConstants[1],
            other.blendConstants[2],
            other.blendConstants[3]
        }
    }
{}

constexpr ColorBlendState::ColorBlendState(ColorBlendState&& other) noexcept:
    VkPipelineColorBlendStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        other.pNext,
        other.flags,
        other.logicOpEnable,
        other.logicOp,
        other.attachmentCount,
        other.pAttachments,
        {
            other.blendConstants[0],
            other.blendConstants[1],
            other.blendConstants[2],
            other.blendConstants[3]
        }
    }
{
    other.pNext = nullptr;
    other.flags = 0;
    other.logicOpEnable = VK_FALSE;
    other.logicOp = VK_LOGIC_OP_CLEAR;
    other.attachmentCount = 0;
    other.pAttachments = nullptr;
    other.blendConstants[0] = 0.f;
    other.blendConstants[1] = 0.f;
    other.blendConstants[2] = 0.f;
    other.blendConstants[3] = 0.f;
}

constexpr hash_t ColorBlendState::hash() const noexcept
{
    return core::hashArgs(
        sType,
        flags,
        logicOpEnable,
        logicOp,
        attachmentCount,
        pAttachments->blendEnable,
        pAttachments->srcColorBlendFactor,
        pAttachments->dstColorBlendFactor,
        pAttachments->colorBlendOp,
        pAttachments->srcAlphaBlendFactor,
        pAttachments->dstAlphaBlendFactor,
        pAttachments->alphaBlendOp,
        pAttachments->colorWriteMask,
        blendConstants[0],
        blendConstants[1],
        blendConstants[2],
        blendConstants[3]);
}

constexpr bool ColorBlendState::operator==(const ColorBlendState& other) const noexcept
{
    return (flags == other.flags) &&
        (logicOpEnable == other.logicOpEnable) &&
        (logicOp == other.logicOp) &&
        (attachmentCount == other.attachmentCount) &&
        (pAttachments->blendEnable == other.pAttachments->blendEnable) &&
        (pAttachments->srcColorBlendFactor == other.pAttachments->srcColorBlendFactor) &&
        (pAttachments->dstColorBlendFactor == other.pAttachments->dstColorBlendFactor) &&
        (pAttachments->colorBlendOp == other.pAttachments->colorBlendOp) &&
        (pAttachments->srcAlphaBlendFactor == other.pAttachments->srcAlphaBlendFactor) &&
        (pAttachments->dstAlphaBlendFactor == other.pAttachments->dstAlphaBlendFactor) &&
        (pAttachments->alphaBlendOp == other.pAttachments->alphaBlendOp) &&
        (pAttachments->colorWriteMask == other.pAttachments->colorWriteMask) &&
        (blendConstants[0] == other.blendConstants[0]) &&
        (blendConstants[1] == other.blendConstants[1]) &&
        (blendConstants[2] == other.blendConstants[2]) &&
        (blendConstants[3] == other.blendConstants[3]);
}
} // namespace magma
