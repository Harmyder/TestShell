#pragma once

#include <Math/Vector.h>

namespace Common {
    XMVECTOR_ALIGNMENT class Matrix4
    {
    public:
        Matrix4() {}
        Matrix4(Vector4 x, Vector4 y, Vector4 z, Vector4 w) { m_.r[0] = x; m_.r[1] = y; m_.r[2] = z; m_.r[3] = w; }
        Matrix4(const Matrix4& mat) { m_ = mat.m_; }
        explicit Matrix4(const XMMATRIX& mat) { m_ = mat; }
        explicit Matrix4(OneTag) { m_ = XMMatrixIdentity(); }
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

        Vector4 operator* (Vector3 vec) const { return Vector4(XMVector3Transform(vec, m_)); }
        Vector4 operator* (Vector4 vec) const { return Vector4(XMVector4Transform(vec, m_)); }
        Matrix4 operator* (const Matrix4& mat) const { return Matrix4(XMMatrixMultiply(mat, m_)); }

        static Matrix4 MakeScale(float scale) { return Matrix4(XMMatrixScaling(scale, scale, scale)); }
        static Matrix4 MakeScale(Vector3 scale) { return Matrix4(XMMatrixScalingFromVector(scale)); }

    private:
        XMMATRIX m_;
    };
}
