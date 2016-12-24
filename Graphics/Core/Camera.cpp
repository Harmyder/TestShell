#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;
using namespace std;

namespace Graphics
{
    void Camera::SetAffineTransform(FXMMATRIX affine) {
        SetLookDirection(affine.r[2], affine.r[1]);
        SetPosition(affine.r[3]);
    }

    void Camera::Update() {
        view_ = XMMatrixTranspose(cameraRotation_);
        view_.r[2] = g_XMIdentityR2;
        view_.r[3] = XMVectorSetW(XMVector3Transform(-cameraTranslation_, view_), 1.f);
    }

    void Camera::SetLookDirection(FXMVECTOR forward, FXMVECTOR up) {
        XMFLOAT4 v;
        DirectX::XMStoreFloat4(&v, forward);
        DirectX::XMStoreFloat4(&v, up);
        XMVECTOR right = XMVector3Cross(forward, up);
        cameraRotation_ = XMMATRIX(right, XMVector3Cross(right, forward), forward, g_XMIdentityR3);
    }

    void Camera::SetPosition(FXMVECTOR p) {
        cameraTranslation_ = p;
    }

    void Camera::UpdateProjMatrix() {
        float y = 1.f / std::tanf(fov_ / 2.f);
        float x = aspectRatio_ * y;

        float A = f_ / (f_ - n_);
        float B = A * (-n_);

        proj_ = XMMATRIX(
            XMVECTOR{ x,    0.f,  0.f,  0.f },
            XMVECTOR{ 0.f,  y,    0.f,  0.f },
            XMVECTOR{ 0.f,  0.f,  A,    1.f },
            XMVECTOR{ 0.f,  0.f,  B,    0.f }
        );
    }
}
