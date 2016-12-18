#pragma once

#include <DirectXMath.h>

namespace Viewer
{
    __declspec(align(16)) class Camera
    {
    public:
        void SetTransform(const DirectX::XMMATRIX& tranform);

    private:
        DirectX::XMMATRIX transform_;
    };
}
