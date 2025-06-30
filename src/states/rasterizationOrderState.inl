namespace magma
{
constexpr RasterizationOrderState::RasterizationOrderState(const RasterizationState& state,
    const VkRasterizationOrderAMD rasterizationOrder) noexcept:
    RasterizationState(state),
    rasterizationOrderInfo{
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_RASTERIZATION_ORDER_AMD,
        state.pNext,
        rasterizationOrder
    }
{
    pNext = &rasterizationOrderInfo;
}

constexpr RasterizationOrderState::RasterizationOrderState(const RasterizationOrderState& other) noexcept:
    RasterizationState(other),
    rasterizationOrderInfo(other.rasterizationOrderInfo)
{
    pNext = &rasterizationOrderInfo;
}

constexpr hash_t RasterizationOrderState::hash() const noexcept
{
    hash_t hash = RasterizationState::hash();
    return core::hashCombine(hash, core::hashArgs(
        rasterizationOrderInfo.sType,
        rasterizationOrderInfo.rasterizationOrder));
}

constexpr bool RasterizationOrderState::operator==(const RasterizationOrderState& other) const noexcept
{
    return RasterizationState::operator==(other) &&
        (rasterizationOrderInfo.rasterizationOrder == other.rasterizationOrderInfo.rasterizationOrder);
}
} // namespace magma
