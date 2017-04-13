#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include "Core\Lighting.h"

namespace Graphics {
    template<class E> class ConstantBuffer;
    template<class E> class UploadBuffer;
    class CommandQueue;

    struct PerObjConsts
    {
        DirectX::XMFLOAT4X3 World;
        uint32 MaterialIndex;
        uint32 pad1, pad2, pad3;
    };

    struct InstanceData
    {
        DirectX::XMFLOAT4X3 World;
        uint32 MaterialIndex;
        uint32 pad1, pad2, pad3;
    };

    struct DirectionalLightConsts
    {
        DirectX::XMFLOAT3 Strength;
        float pad1;
        DirectX::XMFLOAT3 Direction;
        float pad2;
    };

    struct PointLightConsts
    {
        DirectX::XMFLOAT3 Strength;
        float Range;
        DirectX::XMFLOAT3 Position;
        float pad1;
        DirectX::XMFLOAT3 Attenuation;
        float pad2;
    };

    struct SpotLightConsts
    {
        DirectX::XMFLOAT3 Strength;
        float Range;
        DirectX::XMFLOAT3 Position;
        float Spot;
        DirectX::XMFLOAT3 Direction;
        float pad1;
        DirectX::XMFLOAT3 Attenuation;
        float pad2;
    };

    struct PerPassConsts
    { 
        DirectX::XMFLOAT4X4 View;
        DirectX::XMFLOAT4X4 Proj;
        DirectX::XMFLOAT4X4 ViewProj;
        DirectX::XMFLOAT3   EyePosW;
        int pad1;
        DirectionalLightConsts DirLights[MAX_DIR_LIGHTS_COUNT];
        PointLightConsts       PntLights[MAX_PNT_LIGHTS_COUNT];
        SpotLightConsts        SptLights[MAX_SPT_LIGHTS_COUNT];
        uint32 DirLightsCount;
        uint32 PntLightsCount;
        uint32 SptLightsCount;
        int pad2;
    };

    void FillPerPassLights(const LightsHolder& lightsHolder, PerPassConsts& perPass);

    struct PerMatConsts
    {
        DirectX::XMFLOAT4 Ambient;
        DirectX::XMFLOAT4 Diffuse;
        DirectX::XMFLOAT4 Specular;
        float FresnelR0;
        float Roughness;
        float pad0;
        float pad1;
    };

    class FrameResource
    {
    public:
        FrameResource(uint32 passesCount, uint32 objsCount, uint32 matsCount, uint32 instsCount, const std::wstring& suffix);
        ~FrameResource();

        uint64 Fence = 0;
        std::unique_ptr<ConstantBuffer<PerPassConsts>> passCB;
        std::unique_ptr<ConstantBuffer<PerObjConsts>> objCB;
        std::unique_ptr<UploadBuffer<PerMatConsts>> matBuffer;
        std::unique_ptr<UploadBuffer<InstanceData>> instBuffer;
    };

    class FrameResources {
    public:
        FrameResources(uint32 count, uint32 passesCount, uint32 objsCount, uint32 matsCount, uint32 texsCount, uint32 instsCount);
        ~FrameResources();

        void AdvanceFrame() { currentIndex_ = ++currentIndex_ % Count(); }
        uint32 GetCurrentIndex() { return currentIndex_; }
        FrameResource& GetCurrentFrameResource() { return *resources_[currentIndex_]; }
        FrameResource& GetFrameResource(uint_t index) { return *resources_[index]; }
        uint32 Count() const { return (uint32)resources_.size(); }

        uint_t CalcPassCbSize() const { return Utility::CalcConstBufSize(sizeof(PerPassConsts)); }
        uint_t CalcObjCbSize() const { return Utility::CalcConstBufSize(sizeof(PerObjConsts)); }

        const uint32 PassesCountLimit;
        const uint32 ObjsCountLimit;
        const uint32 MatsCountLimit;
        const uint32 TexsCountLimit;
        const uint32 InstancesCountLimit;

        uint32 AcquireInstsRange(uint32 count) {
            assert(nextFreeInst + count < InstancesCountLimit);
            const uint32 res = nextFreeInst;
            nextFreeInst += count;
            return res;
        }

    private:
        std::vector<std::unique_ptr<FrameResource>> resources_;
        uint32 currentIndex_ = 0;
        uint32 nextFreeInst = 0;
    };
}
