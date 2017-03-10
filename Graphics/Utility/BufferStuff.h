#pragma once

namespace Graphics
{
    class LightsHolder;
    
namespace Utility
{
    // every free index stores next index of free index
    // every taken index stores previous free index
    class FreeIndices {
    public:
        FreeIndices(uint32 sceneObjectsCountLimit);

        uint32 OccupyIndex();
        void FreeIndex(uint32 index);

        uint32 FirstTakenIndex() { return NextTakenIndexInternal(0) - 1; }
        uint32 NextTakenIndex(uint32 index) { return NextTakenIndexInternal(index + 1) - 1; }

    private:
        bool IsFreeInternal(uint32 internalIndex) { return indices_[internalIndex] > internalIndex; }
        uint32 NextTakenIndexInternal(uint32 internalIndex);

    private:
        std::vector<uint32> indices_;
    };

    class BufferEntryDirty {
    public:
        BufferEntryDirty(uint32 bufferIndex) :
            bufferIndex_(bufferIndex),
            dirtyFramesCount_(0)
        {}

        uint32 BufferIndex() const { return bufferIndex_; }
        bool IsDirty() const { return dirtyFramesCount_ != 0; }
        void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

    protected:
        void SetAllFramesDirty();

    private:
        friend class LightsHolder;

        uint32 bufferIndex_;
        uint32 dirtyFramesCount_;
    };
}
}
