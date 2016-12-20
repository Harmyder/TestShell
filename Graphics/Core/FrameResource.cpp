#include "stdafx.h"
#include "FrameResource.h"

#include "SDK\UploadBuffer.h"
#include "SDK\CommandQueue.h"

using namespace std;

namespace Graphics
{
    FrameResource::FrameResource(uint32 passesCount, uint32 objsCount) {
        passCB = make_unique<ConstantBuffer>(passesCount, sizeof(PerPassConsts));
        objCB = make_unique<ConstantBuffer>(objsCount, sizeof(PerObjConsts));
    }

    FrameResource::~FrameResource() {}
}