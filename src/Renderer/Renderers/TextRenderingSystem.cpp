#include "TextRenderingSystem.h"

void TextBufferBlock::reallocateBlock(const int overflow)
{
    ShaderStorageBuffer newCache;

    const int newSize = static_cast<int>(block.getAllocatedBytes() + overflow * sizeof(CharInfo) * 1.25f);

    newCache.allocate(newSize, BufferUsage::DYNAMIC, nullptr);

    const auto srcBuffer = block.map(BufferAccessMode::READ_ONLY);
    const auto* srcPtr = srcBuffer.get<CharInfo>();

    const auto destinationBuffer = newCache.map(BufferAccessMode::WRITE_ONLY);
    auto* dstPtr = destinationBuffer.get<CharInfo>();

    int dstIndex = 0;

    for (auto &spot : hashes | std::views::values) {
        auto& [index, length] = spot;

        auto* ptr = srcPtr + index;

        memcpy(dstPtr, ptr, length * sizeof(CharInfo));
        dstPtr += length;

        spot.index = dstIndex;
        dstIndex += length;
    }
    highestIndex = dstIndex;

    freeSpots.clear();
    freeSpots.emplace_back(highestIndex, newSize / sizeof(CharInfo) - highestIndex);
    block = std::move(newCache);
}

TextBufferBlock::FreeSpot TextBufferBlock::findFreeSpot(const int length)
{
    const auto it = std::ranges::find_if(freeSpots,
        [length](const FreeSpot& spot) {
        return spot.length >= length;
    });

    if (it != freeSpots.end()) {
        FreeSpot foundSpot = *it;
        freeSpots.erase(it);

        FreeSpot result{};
        result.index = foundSpot.index;
        result.length = length;

        FreeSpot remainingSpot{};
        remainingSpot.index = foundSpot.index + length;
        remainingSpot.length = foundSpot.length - length;

        if (remainingSpot.length > 0) {
            freeSpots.push_back(remainingSpot);
        }
        if (result.index + result.length > highestIndex) highestIndex = result.index + result.length;

        return result;
    }
    reallocateBlock(length);
    return findFreeSpot(length);
}

void TextBufferBlock::removeFreeSpot(const std::unordered_map<size_t, FreeSpot>::iterator &iterator) {
    freeSpots.push_back(iterator->second);

    if (iterator->second.length != 0) {
        const auto mapped = block.map(
            BufferAccessBit::WRITE,
            iterator->second.index,
            iterator->second.length,
            sizeof(CharInfo)
        );
        auto* ptr = mapped.get<CharInfo>();

        for (int i = 0; i < iterator->second.length; ++i) {
            ptr->visible = false;
            ++ptr;
        }
    }
    hashes.erase(iterator);
}

void TextBufferBlock::removeFreeSpotBatchedPointer(CharInfo *basePointer, const std::vector<size_t> &spots)
{
    for (auto &spot : spots) {
        auto& [index, length] = hashes[spot];

        auto* ptr = basePointer + index;

        for (int i = 0; i < length; ++i) {
            ptr->visible = false;
            ++ptr;
        }
        hashes.erase(spot);
        freeSpots.emplace_back(index, length);
    }
}
