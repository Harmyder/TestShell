#pragma once

namespace Graphics
{
namespace Utility
{
    // every free index stores next index of free index
    // every taken index stores previous free index
    class FreeIndices {
    public:
        FreeIndices(uint32 sceneObjectsCountLimit);

        uint32 AcquireIndex();
        void ReleaseIndex(uint32 index);

        uint32 FirstTakenIndex() { return NextTakenIndexInternal(0) - 1; }
        uint32 NextTakenIndex(uint32 index) { return NextTakenIndexInternal(index + 1) - 1; }

    private:
        bool IsFreeInternal(uint32 internalIndex) { return indices_[internalIndex] > internalIndex; }
        uint32 NextTakenIndexInternal(uint32 internalIndex);

    private:
        std::vector<uint32> indices_;
    };
}
}
