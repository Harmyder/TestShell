#include "stdafx.h"

#include "Camera.h"

using namespace DirectX;

namespace Viewer
{
    void Camera::SetTransform(const XMMATRIX& transform) {
        transform_ = transform;
    }
}