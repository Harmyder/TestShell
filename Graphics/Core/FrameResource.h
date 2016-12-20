#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <Pile/Math/XmFloatHelper.h>

namespace Graphics {
    class ConstantBuffer;
    class CommandQueue;

    struct PerObjConsts
    {
        DirectX::XMFLOAT4X4 World = Pile::Identity4x4();
    };

    struct PerPassConsts
    { 
        DirectX::XMFLOAT4X4 View = Pile::Identity4x4();
        DirectX::XMFLOAT4X4 Proj = Pile::Identity4x4();
        DirectX::XMFLOAT4X4 ViewProj = Pile::Identity4x4();
        DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    };

    class FrameResource
    {
        friend class Graphics;
    public:
        FrameResource(uint32 passesCount, uint32 objsCount);
        ~FrameResource();

        uint64 Fence;
        std::unique_ptr<ConstantBuffer> passCB;
        std::unique_ptr<ConstantBuffer> objCB;
    };
}
