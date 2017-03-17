#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#define XMVECTOR_ALIGNMENT __declspec(align(16))

namespace Common {
    enum ZeroTag { kZero };
    enum IdentityTag { kOne, kIdentity };
    enum InfinityTag { kInfinity };
    enum XUnitTag { kXUnit };
    enum YUnitTag { kYUnit };
    enum ZUnitTag { kZUnit };
    enum WUnitTag { kWUnit };

    XMVECTOR_ALIGNMENT class Scalar {
    public:
        Scalar() {}
        Scalar(const Scalar& s) { v_ = s; }
        Scalar(float f) { v_ = XMVectorReplicate(f); }
        explicit Scalar(FXMVECTOR vec) { v_ = vec; }
        explicit Scalar(ZeroTag) { v_ = XMVectorZero(); }
        explicit Scalar(IdentityTag) { v_ = XMVectorSplatOne(); }

        operator XMVECTOR() const { return v_; }
        operator float() const { return XMVectorGetX(v_); }

    private:
        XMVECTOR v_;
    };

    inline Scalar operator- (Scalar s) { return Scalar(XMVectorNegate(s)); }
    inline Scalar operator+ (Scalar s1, Scalar s2) { return Scalar(XMVectorAdd(s1, s2)); }
    inline Scalar operator- (Scalar s1, Scalar s2) { return Scalar(XMVectorSubtract(s1, s2)); }
    inline Scalar operator* (Scalar s1, Scalar s2) { return Scalar(XMVectorMultiply(s1, s2)); }
    inline Scalar operator/ (Scalar s1, Scalar s2) { return Scalar(XMVectorDivide(s1, s2)); }
    inline Scalar operator+ (Scalar s1, float s2) { return s1 + Scalar(s2); }
    inline Scalar operator- (Scalar s1, float s2) { return s1 - Scalar(s2); }
    inline Scalar operator* (Scalar s1, float s2) { return s1 * Scalar(s2); }
    inline Scalar operator/ (Scalar s1, float s2) { return s1 / Scalar(s2); }
    inline Scalar operator+ (float s1, Scalar s2) { return Scalar(s1) + s2; }
    inline Scalar operator- (float s1, Scalar s2) { return Scalar(s1) - s2; }
    inline Scalar operator* (float s1, Scalar s2) { return Scalar(s1) * s2; }
    inline Scalar operator/ (float s1, Scalar s2) { return Scalar(s1) / s2; }
}
