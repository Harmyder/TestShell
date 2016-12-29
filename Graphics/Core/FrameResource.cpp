#include "stdafx.h"
#include "FrameResource.h"

#include "SDK\UploadBuffer.h"
#include "SDK\CommandQueue.h"

using namespace std;

namespace Graphics
{
    FrameResource::FrameResource(uint32 passesCount, uint32 objsCount, uint32 matsCount) {
        passCB = make_unique<ConstantBuffer>(L"fr passes", passesCount, sizeof(PerPassConsts));
        objCB = make_unique<ConstantBuffer>(L"fr objs", objsCount, sizeof(PerObjConsts));
        matCB = make_unique<ConstantBuffer>(L"fr mats", matsCount, sizeof(MatConsts));
    }

    FrameResource::~FrameResource() {}
}