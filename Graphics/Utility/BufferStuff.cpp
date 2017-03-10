#include "stdafx.h"
#include "Utility\BufferStuff.h"

#include "Core\GraphicsCore.h"
#include "Core\FrameResource.h"

namespace Graphics
{
namespace Utility
{
    void BufferEntryDirty::SetAllFramesDirty() {
        dirtyFramesCount_ = (uint32)GraphicsCore::GetInstance().GetFrameResources()->Count();
    }

    FreeIndices::FreeIndices(uint32 sceneObjectsCountLimit) :
        // One more because first index serves to point to first free real index
        indices_(sceneObjectsCountLimit + 1)
    {
        std::iota(std::begin(indices_), std::end(indices_), 1);
    }

    uint32 FreeIndices::OccupyIndex() {
        const uint32 res = indices_[0];
        indices_[0] = indices_[res];
        indices_[res] = 0;
        return res - 1;
    }

    void FreeIndices::FreeIndex(uint32 index) {
        const auto internalIndex = index + 1;
        const uint32 prevFree = indices_[internalIndex];
        indices_[internalIndex] = indices_[prevFree];
        indices_[prevFree] = internalIndex;
        auto nextIndex = internalIndex + 1;
        while (nextIndex < indices_.size() && indices_[nextIndex] < nextIndex) {
            indices_[nextIndex++] = internalIndex;
        }
    }

    uint32 FreeIndices::NextTakenIndexInternal(uint32 internalIndex) {
        while (++internalIndex < indices_.size() && IsFreeInternal(internalIndex));
        return internalIndex;
    }
}
}