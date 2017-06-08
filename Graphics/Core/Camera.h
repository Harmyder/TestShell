#pragma once
#include "Common\DirectXInclude.h"

namespace Graphics
{
    __declspec(align(64)) class Camera
    {
    public:
        void __vectorcall SetAffineTransform(FXMMATRIX affine);

        void SetPerspective(float heightOverWidth, float fov, float nearCP, float farCP) {
            aspectRatio_ = heightOverWidth;
            fov_ = fov;
            n_ = nearCP;
            f_ = farCP;
            UpdateProjMatrix();
        }

        void Update();

        XMMATRIX GetProjMatrix() { return proj_; }
        XMMATRIX GetViewMatrix() { return view_; }
        XMMATRIX GetInvViewMatrix() { return invView_; }
        XMVECTOR GetEyePos() { return cameraTranslation_; }

    private:
        void __vectorcall SetLookDirection(FXMVECTOR forward, FXMVECTOR up);
        void __vectorcall SetPosition(FXMVECTOR worldPos);
        void UpdateProjMatrix();

    private:
        XMMATRIX view_;
        XMMATRIX invView_;
        XMMATRIX proj_;

        XMMATRIX cameraRotation_;
        XMVECTOR cameraTranslation_;

        float aspectRatio_;
        float fov_;
        float n_;
        float f_;
    };
}
