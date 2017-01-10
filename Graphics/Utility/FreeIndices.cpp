#include "stdafx.h"
#include "Utility\FreeIndices.h"

namespace Graphics
{
namespace Utility
{
    FreeIndices::FreeIndices(uint32 sceneObjectsCountLimit) :
        // One more because first index serves to point to first free real index
        indices_(sceneObjectsCountLimit + 1)
    {
        std::iota(std::begin(indices_), std::end(indices_), 1);
    }

    uint32 FreeIndices::AcquireIndex() {
        const uint32 res = indices_[0];
        indices_[0] = indices_[res];
        indices_[res] = 0;
        return res - 1;
    }

    void FreeIndices::ReleaseIndex(uint32 index) {
        auto internalIndex = index + 1;
        const uint32 prevFree = indices_[internalIndex];
        indices_[internalIndex] = indices_[prevFree];
        indices_[prevFree] = internalIndex;
    }

    uint32 FreeIndices::NextTakenIndexInternal(uint32 internalIndex) {
        while (++internalIndex < indices_.size() && IsFreeInternal(internalIndex));
        return internalIndex;
    }
}
}