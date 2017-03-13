#pragma once

#include "Math/Matrix.h"

namespace Common {
    // Linear part of transform can contain scale, shear, reflection, rotation.
    XMVECTOR_ALIGNMENT class AffineTransform {
    public:
        AffineTransform(Vector3 x, Vector3 y, Vector3 z, Vector3 w) : linear_(x, y, z), translation_(w) {}
        AffineTransform(Vector3 translate) : linear_(kIdentity), translation_(translate) {}
        AffineTransform(const Matrix3& mat, Vector3 translate = Vector3(kZero)) : linear_(mat), translation_(translate) {}
//        AffineTransform(Quaternion rot, Vector3 translate = Vector3(kZero)) : linear_(rot), translation_(translate) {}
        AffineTransform(IdentityTag) : linear_(kIdentity), translation_(kZero) {}
        explicit AffineTransform(const XMMATRIX& mat) : linear_(mat), translation_(mat.r[3]) {}

        operator XMMATRIX() const { return (XMMATRIX&)*this; }
        XMFLOAT4X3 Store() const { XMFLOAT4X3 t; XMStoreFloat4x3(&t, (XMMATRIX&)*this); return t; }

        void SetX(Vector3 x) { linear_.SetX(x); }
        void SetY(Vector3 y) { linear_.SetY(y); }
        void SetZ(Vector3 z) { linear_.SetZ(z); }
        void SetTranslation(Vector3 w) { translation_ = w; }

        Vector3 GetX() const { return linear_.GetX(); }
        Vector3 GetY() const { return linear_.GetY(); }
        Vector3 GetZ() const { return linear_.GetZ(); }
        Vector3 GetTranslation() const { return translation_; }
        const Matrix3& GetLinear() const { return (const Matrix3&)*this; }

        static AffineTransform MakeXRotation(float angle) { return AffineTransform(Matrix3::MakeXRotation(angle)); }
        static AffineTransform MakeYRotation(float angle) { return AffineTransform(Matrix3::MakeYRotation(angle)); }
        static AffineTransform MakeZRotation(float angle) { return AffineTransform(Matrix3::MakeZRotation(angle)); }
        static AffineTransform MakeScale(float scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static AffineTransform MakeScale(Vector3 scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static AffineTransform MakeTranslation(Vector3 translate) { return AffineTransform(translate); }

        Vector3 operator* (Vector3 vec) const { return linear_ * vec + translation_; }
        AffineTransform operator* (const AffineTransform& mat) const {
            return AffineTransform(linear_ * mat.linear_, *this * mat.GetTranslation());
        }

    private:
        Matrix3 linear_;
        Vector3 translation_;
    };
}