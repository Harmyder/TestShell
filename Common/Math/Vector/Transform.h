#pragma once

#include "Consts.h"
#include "Math/Vector/Matrix.h"
#include "Math/Vector/Quaternion.h"

namespace Common {
    class AffineTransform;

    // Orthogonal non-scaling transform
    XMVECTOR_ALIGNMENT class OrthogonalTransform {
    public:
        OrthogonalTransform() : rotation_(kIdentity), translation_(kZero) {}
        OrthogonalTransform(Quaternion rotate) : rotation_(rotate), translation_(kZero) {}
        OrthogonalTransform(Vector3 translate) : rotation_(kIdentity), translation_(translate) {}
        OrthogonalTransform(Quaternion rotate, Vector3 translate) : rotation_(rotate), translation_(translate) {}
        OrthogonalTransform(const Matrix3& m) : rotation_(m), translation_(kZero) {}
        OrthogonalTransform(const Matrix3& m, Vector3 translate) : rotation_(m), translation_(translate) {}
        OrthogonalTransform(IdentityTag) : rotation_(kIdentity), translation_(kZero) {}
        explicit OrthogonalTransform(const XMMATRIX& m) : rotation_(kIdentity), translation_(kInfinity) { *this = OrthogonalTransform(Matrix3(m), Vector3(m.r[3])); }

        void SetRotation(Quaternion q) { rotation_ = q; }
        void SetTranslation(Vector3 v) { translation_ = v; }

        Quaternion GetRotation() const { return rotation_; }
        Vector3 GetTranslation() const { return translation_; }

        static OrthogonalTransform MakeXRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kXUnit), angle)); }
        static OrthogonalTransform MakeYRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kYUnit), angle)); }
        static OrthogonalTransform MakeZRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kZUnit), angle)); }
        static OrthogonalTransform MakeTranslation(float x, float y, float z) { return OrthogonalTransform(Vector3(x, y, z)); }
        static OrthogonalTransform MakeTranslation(Vector3 translate) { return OrthogonalTransform(translate); }

        Vector3 operator* (Vector3 vec) const { return rotation_ * vec + translation_; }
        OrthogonalTransform operator* (const OrthogonalTransform& xform) const {
            return OrthogonalTransform(rotation_ * xform.rotation_, rotation_ * xform.translation_ + translation_);
        }

        OrthogonalTransform operator~ () const {
            Quaternion invertedRotation = ~rotation_;
            return OrthogonalTransform(invertedRotation, invertedRotation * -translation_);
        }
    private:
        friend AffineTransform OrthoToAffine(OrthogonalTransform ortho);

        Quaternion rotation_;
        Vector3 translation_;
    };

    // Linear part of transform can contain scale, shear, reflection, rotation.
    XMVECTOR_ALIGNMENT class AffineTransform {
    public:
        AffineTransform() : linear_(kIdentity), translation_(kZero) {}
        AffineTransform(Vector3 x, Vector3 y, Vector3 z, Vector3 w) : linear_(x, y, z), translation_(w) {}
        AffineTransform(Vector3 translate) : linear_(kIdentity), translation_(translate) {}
        AffineTransform(const Matrix3& mat, Vector3 translate = Vector3(kZero)) : linear_(mat), translation_(translate) {}
        AffineTransform(Quaternion rot, Vector3 translate = Vector3(kZero)) : linear_(rot), translation_(translate) {}
        AffineTransform(IdentityTag) : linear_(kIdentity), translation_(kZero) {}
        explicit AffineTransform(const XMMATRIX& mat) : linear_(mat), translation_(mat.r[3]) {}

        operator XMMATRIX() const { return (XMMATRIX&)*this; }
        XMFLOAT4X3 Store4x3() const { XMFLOAT4X3 t; XMStoreFloat4x3(&t, (XMMATRIX&)*this); return t; }
        XMFLOAT4X4 Store4x4() const { XMFLOAT4X4 t; XMStoreFloat4x4(&t, (XMMATRIX&)*this); return t; }

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

    inline AffineTransform OrthoToAffine(OrthogonalTransform ortho) { return AffineTransform(ortho.rotation_, ortho.translation_); }
}