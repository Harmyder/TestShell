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

    class BufferEntry {
    public:
        BufferEntry() {}
        BufferEntry(uint32 bufferIndex) : bufferIndex_(bufferIndex) {}

        void InitBufferIndex(uint32 bufferIndex) { assert(bufferIndex_ == kNoIndex); bufferIndex_ = bufferIndex; }
        uint32 BufferIndex() const { return bufferIndex_; }
        bool HasIndex() const { return bufferIndex_ != kNoIndex; }

    private:
        friend class LightsHolder;

        static constexpr uint32 kNoIndex = (uint32)-1;
        uint32 bufferIndex_ = kNoIndex;
    };

    class BufferEntryDirty : public BufferEntry {
    public:
        using BufferEntry::BufferEntry;

        bool IsDirty() const { return dirtyFramesCount_ != 0; }
        void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

        void SetAllFramesDirty();

    private:
        uint32 dirtyFramesCount_ = 0;
    };
}
}
