#pragma once
#include "Pile\DirectXInclude.h"

namespace Graphics
{
    __declspec(align(64)) class Camera
    {
    public:
        void __vectorcall SetAffineTransform(FXMMATRIX affine);

        void SetAspectRatio(float heightOverWidth) { aspectRatio_ = heightOverWidth; UpdateProjMatrix(); }
        void SetVerticalFOV(float fov) { fov_ = fov; UpdateProjMatrix(); }
        void SetNearFarClipPlanes(float nearCP, float farCP) { n_ = nearCP; f_ = farCP; UpdateProjMatrix(); }

        void Update();

        XMMATRIX GetProjMatrix() { return proj_; }
        XMMATRIX GetViewMatrix() { return view_; }
        XMVECTOR GetEyePos() { return cameraTranslation_; }

    private:
        void __vectorcall SetLookDirection(FXMVECTOR forward, FXMVECTOR up);
        void __vectorcall SetPosition(FXMVECTOR worldPos);
        void UpdateProjMatrix();

    private:
        XMMATRIX view_;
        XMMATRIX proj_;

        XMMATRIX cameraRotation_;
        XMVECTOR cameraTranslation_;

        float aspectRatio_;
        float fov_;
        float n_;
        float f_;
    };
}
