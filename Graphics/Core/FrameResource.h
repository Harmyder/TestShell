#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include "Pile/Math/XmFloatHelper.h"

namespace Graphics {
    class ConstantBuffer;
    class CommandQueue;

    struct PerObjConsts
    {
        DirectX::XMFLOAT4X4 World = Pile::Identity4x4();
    };

    struct PerPassConsts
    { 
        DirectX::XMFLOAT4X4 View;
        DirectX::XMFLOAT4X4 Proj;
        DirectX::XMFLOAT4X4 ViewProj;
        DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    };

    struct MatConsts
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
        FrameResource(uint32 passesCount, uint32 objsCount, uint32 matsCount);
        ~FrameResource();

        uint64 Fence;
        std::unique_ptr<ConstantBuffer> matCB;
        std::unique_ptr<ConstantBuffer> passCB;
        std::unique_ptr<ConstantBuffer> objCB;
    };
}
