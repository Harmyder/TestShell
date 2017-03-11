#pragma once

#include <limits>
#include "Math/Scalar.h"

namespace Common {
    XMVECTOR_ALIGNMENT class Vector3
    {
    public:
        Vector3(float x, float y, float z) { v_ = XMVectorSet(x, y, z, w); }
        Vector3(Scalar s) { v_ = s; }
        Vector3(FXMVECTOR v) { v_ = v; }
        Vector3(const Vector3& o) { v_ = o.v_; }
        Vector3(ZeroTag) { v_ = XMVectorSet(0.f, 0.f, 0.f, w); }
        Vector3(OneTag) { v_ = XMVectorSet(1.f, 1.f, 1.f, w); }
        Vector3(XUnitTag) { v_ = XMVectorSet(1.f, 0.f, 0.f, w); }
        Vector3(XUnitTag) { v_ = XMVectorSet(0.f, 1.f, 0.f, w); }
        Vector3(XUnitTag) { v_ = XMVectorSet(0.f, 0.f, 1.f, w); }

        operator XMVECTOR() const { return v_; }

        Scalar GetX() const { return Scalar(XMVectorSplatX(v_)); }
        Scalar GetY() const { return Scalar(XMVectorSplatY(v_)); }
        Scalar GetZ() const { return Scalar(XMVectorSplatZ(v_)); }
        void SetX(Scalar x) { v_ = XMVectorPermute<4, 1, 2, 3>(v_, x); }
        void SetY(Scalar y) { v_ = XMVectorPermute<0, 5, 2, 3>(v_, y); }
        void SetZ(Scalar z) { v_ = XMVectorPermute<0, 1, 6, 3>(v_, z); }

        Vector3 operator- () const { return Vector3(XMVectorNegate(v_)); }
        Vector3 operator+ (Vector3 o) const { return Vector3(XMVectorAdd(v_, o)); }
        Vector3 operator- (Vector3 o) const { return Vector3(XMVectorSubtract(v_, o)); }
        Vector3 operator* (Vector3 o) const { return Vector3(XMVectorMultiply(v_, o)); }
        Vector3 operator/ (Vector3 o) const { return Vector3(XMVectorDivide(v_, o)); }
        Vector3 operator* (Scalar  s) const { return *this * Vector3(s); }
        Vector3 operator/ (Scalar  s) const { return *this / Vector3(s); }
        Vector3 operator* (float  f) const { return *this * Scalar(f); }
        Vector3 operator/ (float  f) const { return *this / Scalar(f); }

        Vector3& operator += (Vector3 v) { *this = *this + v; return *this; }
        Vector3& operator -= (Vector3 v) { *this = *this - v; return *this; }
        Vector3& operator *= (Vector3 v) { *this = *this * v; return *this; }
        Vector3& operator /= (Vector3 v) { *this = *this / v; return *this; }

        friend Vector3 operator* (Scalar  v1, Vector3 v2) { return Vector3(v1) * v2; }
        friend Vector3 operator/ (Scalar  v1, Vector3 v2) { return Vector3(v1) / v2; }
        friend Vector3 operator* (float   v1, Vector3 v2) { return Scalar(v1) * v2; }
        friend Vector3 operator/ (float   v1, Vector3 v2) { return Scalar(v1) / v2; }

    private:
        static constexpr float w = std::numeric_limits<float>::infinity();
        XMVECTOR v_;
    };

    XMVECTOR_ALIGNMENT class Vector4
    {
    public:
        Vector4() {}
        Vector4(float x, float y, float z, float w) { v_ = XMVectorSet(x, y, z, w); }
        Vector4(Vector3 xyz, float w) { v_ = XMVectorSetW(xyz, w); }
        Vector4(const Vector4& v) { v_ = v; }
        Vector4(const Scalar& s) { v_ = s; }
        explicit Vector4(Vector3 xyz) { v_ = XMVectorSetZ(xyz, 1.f); }
        explicit Vector4(FXMVECTOR vec) { v_ = vec; }
        explicit Vector4(ZeroTag) { v_ = XMVectorZero(); }
        explicit Vector4(OneTag) { v_ = XMVectorSplatOne(); }
        explicit Vector4(XUnitTag) { v_ = XMVectorSet(1.f, 0.f, 0.f, 0.f); }
        explicit Vector4(YUnitTag) { v_ = XMVectorSet(0.f, 1.f, 0.f, 0.f); }
        explicit Vector4(ZUnitTag) { v_ = XMVectorSet(0.f, 0.f, 1.f, 0.f); }
        explicit Vector4(WUnitTag) { v_ = XMVectorSet(0.f, 0.f, 0.f, 1.f); }

        operator XMVECTOR() const { return v_; }

        Scalar GetX() const { return Scalar(XMVectorSplatX(v_)); }
        Scalar GetY() const { return Scalar(XMVectorSplatY(v_)); }
        Scalar GetZ() const { return Scalar(XMVectorSplatZ(v_)); }
        Scalar GetW() const { return Scalar(XMVectorSplatW(v_)); }
        void SetX(Scalar x) { v_ = XMVectorPermute<4, 1, 2, 3>(v_, x); }
        void SetY(Scalar y) { v_ = XMVectorPermute<0, 5, 2, 3>(v_, y); }
        void SetZ(Scalar z) { v_ = XMVectorPermute<0, 1, 6, 3>(v_, z); }
        void SetW(Scalar w) { v_ = XMVectorPermute<0, 1, 2, 7>(v_, w); }

        Vector4 operator- () const { return Vector4(XMVectorNegate(v_)); }
        Vector4 operator+ (Vector4 v2) const { return Vector4(XMVectorAdd(v_, v2)); }
        Vector4 operator- (Vector4 v2) const { return Vector4(XMVectorSubtract(v_, v2)); }
        Vector4 operator* (Vector4 v2) const { return Vector4(XMVectorMultiply(v_, v2)); }
        Vector4 operator/ (Vector4 v2) const { return Vector4(XMVectorDivide(v_, v2)); }
        Vector4 operator* (Scalar  v2) const { return *this * Vector4(v2); }
        Vector4 operator/ (Scalar  v2) const { return *this / Vector4(v2); }
        Vector4 operator* (float   v2) const { return *this * Scalar(v2); }
        Vector4 operator/ (float   v2) const { return *this / Scalar(v2); }

        void operator*= (float   v2) { *this = *this * Scalar(v2); }
        void operator/= (float   v2) { *this = *this / Scalar(v2); }

        friend Vector4 operator* (Scalar  v1, Vector4 v2) { return Vector4(v1) * v2; }
        friend Vector4 operator/ (Scalar  v1, Vector4 v2) { return Vector4(v1) / v2; }
        friend Vector4 operator* (float   v1, Vector4 v2) { return Scalar(v1) * v2; }
        friend Vector4 operator/ (float   v1, Vector4 v2) { return Scalar(v1) / v2; }

    private:
        XMVECTOR v_;
    };
}
