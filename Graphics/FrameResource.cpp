#include "stdafx.h"
#include "FrameResource.h"

#include "UploadBuffer.h"
#include "CommandQueue.h"

using namespace std;

namespace Graphics
{
    FrameResource::FrameResource(CommandQueue& cq, uint32 passesCount, uint32 objsCount) {
        cmdAlloc_ = cq.AcquireAllocator();

        passCB = make_unique<ConstantBuffer>(passesCount, sizeof(PerPassConsts));
        objCB = make_unique<ConstantBuffer>(objsCount, sizeof(PerObjConsts));
    }

    FrameResource::~FrameResource() {}
}