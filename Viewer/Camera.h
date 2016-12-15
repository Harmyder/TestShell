#pragma once

#include <DirectXMath.h>

namespace Viewer
{
    class Camera
    {
    public:
        void SetTransform(const DirectX::XMMATRIX& tranform);

    private:
        DirectX::XMMATRIX transform_;
    };
}
