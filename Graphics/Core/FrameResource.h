#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include "Pile\Math\XmFloatHelper.h"
#include "Core\Lighting.h"

namespace Graphics {
    class ConstantBuffer;
    class CommandQueue;

    struct PerObjConsts
    {
        DirectX::XMFLOAT4X4 World = Pile::Identity4x4();
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
    };

    class FrameResource
    {
        friend class Graphics;
    public:
        FrameResource(uint32 passesCount, uint_t passCbSize, uint32 objsCount, uint_t objCbSize, uint32 matsCount, uint_t matCbSize);
        ~FrameResource();

        uint64 Fence;
        std::unique_ptr<ConstantBuffer> matCB;
        std::unique_ptr<ConstantBuffer> passCB;
        std::unique_ptr<ConstantBuffer> objCB;
    };

    class FrameResources {
    public:
        FrameResources(uint32 count, uint32 passesCount, uint32 objsCount, uint32 matsCount);
        ~FrameResources();

        FrameResource& GetFrameResource(uint_t i) { return *resources_[i]; }
        uint_t Count() { return resources_.size(); }

        uint_t CalcPassCbSize() const { return Utility::CalcConstBufSize(sizeof(PerPassConsts)); }
        uint_t CalcObjCbSize() const { return Utility::CalcConstBufSize(sizeof(PerObjConsts)); }
        uint_t CalcMatCbSize() const { return Utility::CalcConstBufSize(sizeof(PerMatConsts)); }

        const uint32 PassesCountLimit;
        const uint32 ObjsCountLimit;
        const uint32 MatsCountLimit;

    private:
        std::vector<std::unique_ptr<FrameResource>> resources_;
    };
}
