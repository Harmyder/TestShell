#include "stdafx.h"
#include "FrameResource.h"

#include "SDK\UploadBuffer.h"
#include "SDK\CommandQueue.h"

using namespace std;

namespace Graphics
{
    void FillPerPassLights(const LightsHolder& lightsHolder, PerPassConsts& perPass) {
        perPass.DirLightsCount = lightsHolder.DirLightsCount();
        perPass.PntLightsCount = lightsHolder.PntLightsCount();
        perPass.SptLightsCount = lightsHolder.SptLightsCount();

        for (uint8 i = 0; i < perPass.DirLightsCount; ++i) {
            perPass.DirLights[i].Direction = lightsHolder.DirLights()[i]->Direction();
            perPass.DirLights[i].Strength =  lightsHolder.DirLights()[i]->Strength();
        }
        for (uint8 i = 0; i < perPass.PntLightsCount; ++i) {
            perPass.PntLights[i].Position =    lightsHolder.PntLights()[i]->Position();
            perPass.PntLights[i].Range =       lightsHolder.PntLights()[i]->Range();
            perPass.PntLights[i].Strength =    lightsHolder.PntLights()[i]->Strength();
            perPass.PntLights[i].Attenuation = lightsHolder.PntLights()[i]->Attenuation();
        }
        for (uint8 i = 0; i < perPass.SptLightsCount; ++i) {
            perPass.SptLights[i].Direction =   lightsHolder.SptLights()[i]->Direction();
            perPass.SptLights[i].Position =    lightsHolder.SptLights()[i]->Position();
            perPass.SptLights[i].Range =       lightsHolder.SptLights()[i]->Range();
            perPass.SptLights[i].Strength =    lightsHolder.SptLights()[i]->Strength();
            perPass.SptLights[i].Attenuation = lightsHolder.SptLights()[i]->Attenuation();
            perPass.SptLights[i].Spot =        lightsHolder.SptLights()[i]->Spot();
        }
    }

    FrameResource::FrameResource(uint32 passesCount, uint32 objsCount, uint32 matsCount) {
        passCB = make_unique<ConstantBuffer<PerPassConsts>>(L"fr passes", passesCount);
        objCB  = make_unique<ConstantBuffer<PerObjConsts>>(L"fr objs", objsCount);
        matBuffer  = make_unique<UploadBuffer<PerMatConsts>>(L"fr mats", matsCount, false);
    }

    FrameResource::~FrameResource() {}

    FrameResources::FrameResources(uint32 count, uint32 passesCount, uint32 objsCount, uint32 matsCount) :
        PassesCountLimit(passesCount),
        ObjsCountLimit(objsCount),
        MatsCountLimit(matsCount)
    {
        for (uint_t i = 0; i < count; ++i) {
            resources_.push_back(make_unique<FrameResource>(passesCount, objsCount, matsCount));
        }
    }

    FrameResources::~FrameResources() {}
}