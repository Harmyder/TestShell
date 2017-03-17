#pragma once

#include "Math/Vector.h"

namespace Common {
    XMVECTOR_ALIGNMENT class Matrix3
    {
    public:
        Matrix3(Vector3 x, Vector3 y, Vector3 z) : m_{ x,y,z } {}
        Matrix3(const Matrix3& m) : m_{ m.m_[0], m.m_[1], m.m_[2] } {}
//        Matrix3(Quaternion q) { *this = Matrix3(XMMatrixRotationQuaternion(q)); }
        explicit Matrix3(const XMMATRIX& m) : m_{ Vector3(m.r[0]),  Vector3(m.r[1]), m_[2] = Vector3(m.r[2]) } {}
        explicit Matrix3(IdentityTag) : m_{ Vector3(kXUnit), Vector3(kYUnit), Vector3(kZUnit) } {}
        explicit Matrix3(ZeroTag) : m_{ Vector3(kZero), Vector3(kZero), Vector3(kZero) } {}

        void SetX(Vector3 x) { m_[0] = x; }
        void SetY(Vector3 y) { m_[1] = y; }
        void SetZ(Vector3 z) { m_[2] = z; }

        Vector3 GetX() const { return m_[0]; }
        Vector3 GetY() const { return m_[1]; }
        Vector3 GetZ() const { return m_[2]; }

        static Matrix3 MakeXRotation(float angle) { return Matrix3(XMMatrixRotationX(angle)); }
        static Matrix3 MakeYRotation(float angle) { return Matrix3(XMMatrixRotationY(angle)); }
        static Matrix3 MakeZRotation(float angle) { return Matrix3(XMMatrixRotationZ(angle)); }
        static Matrix3 MakeScale(float scale) { return Matrix3(XMMatrixScaling(scale, scale, scale)); }
        static Matrix3 MakeScale(float sx, float sy, float sz) { return Matrix3(XMMatrixScaling(sx, sy, sz)); }
        static Matrix3 MakeScale(Vector3 scale) { return Matrix3(XMMatrixScalingFromVector(scale)); }

        operator XMMATRIX() const { return (const XMMATRIX&)m_; }

        Vector3 operator* (Vector3 vec) const { return Vector3(XMVector3TransformNormal(vec, *this)); }
        Matrix3 operator* (const Matrix3& mat) const { return Matrix3(*this * mat.GetX(), *this * mat.GetY(), *this * mat.GetZ()); }

    private:
        Vector3 m_[3];
    };

    XMVECTOR_ALIGNMENT class Matrix4
    {
    public:
        Matrix4(Vector4 x, Vector4 y, Vector4 z, Vector4 w) { m_.r[0] = x; m_.r[1] = y; m_.r[2] = z; m_.r[3] = w; }
        Matrix4(const Matrix4& mat) { m_ = mat.m_; }
        explicit Matrix4(const XMMATRIX& mat) { m_ = mat; }
        explicit Matrix4(IdentityTag) { m_ = XMMatrixIdentity(); }
        explicit Matrix4(ZeroTag) { m_.r[0] = m_.r[1] = m_.r[2] = m_.r[3] = XMVectorZero(); }

        Vector4 GetX() const { return Vector4(m_.r[0]); }
        Vector4 GetY() const { return Vector4(m_.r[1]); }
        Vector4 GetZ() const { return Vector4(m_.r[2]); }
        Vector4 GetW() const { return Vector4(m_.r[3]); }

        void SetX(Vector4 x) { m_.r[0] = x; }
        void SetY(Vector4 y) { m_.r[1] = y; }
        void SetZ(Vector4 z) { m_.r[2] = z; }
        void SetW(Vector4 w) { m_.r[3] = w; }

        operator XMMATRIX() const { return m_; }
        XMFLOAT4X3 Store() const { XMFLOAT4X3 t; XMStoreFloat4x3(&t, (XMMATRIX&)*this); return t; }

        Vector4 operator* (Vector3 vec) const { return Vector4(XMVector3Transform(vec, m_)); }
        Vector4 operator* (Vector4 vec) const { return Vector4(XMVector4Transform(vec, m_)); }
        Matrix4 operator* (const Matrix4& mat) const { return Matrix4(XMMatrixMultiply(mat, m_)); }

        static Matrix4 MakeScale(float scale) { return Matrix4(XMMatrixScaling(scale, scale, scale)); }
        static Matrix4 MakeScale(Vector3 scale) { return Matrix4(XMMatrixScalingFromVector(scale)); }

    private:
        XMMATRIX m_;
    };
}
