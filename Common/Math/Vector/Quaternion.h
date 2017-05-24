#pragma once

#include "Math/Vector/Vector.h"

namespace Common
{
    XMVECTOR_ALIGNMENT class Quaternion {
    public:
        Quaternion() { q_ = XMQuaternionIdentity(); }
        Quaternion(const Vector3& axis, const Scalar& angle) { q_ = XMQuaternionRotationAxis(axis, angle); }
        Quaternion(float pitch, float yaw, float roll) { q_ = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
        explicit Quaternion(const XMMATRIX& matrix) { q_ = XMQuaternionRotationMatrix(matrix); }
        explicit Quaternion(FXMVECTOR v) { q_ = v; }
        explicit Quaternion(IdentityTag) { q_ = XMQuaternionIdentity(); }

        operator XMVECTOR() const { return q_; }

        Quaternion operator~ (void) const { return Quaternion(XMQuaternionConjugate(q_)); }
        Quaternion operator- (void) const { return Quaternion(XMVectorNegate(q_)); }

        Quaternion operator* (Quaternion rhs) const { return Quaternion(XMQuaternionMultiply(rhs.q_, q_)); }
        Vector3 operator* (Vector3 rhs) const { return Vector3(XMVector3Rotate(rhs, q_)); }

        Quaternion& operator= (Quaternion rhs) { q_ = rhs; return *this; }
        Quaternion& operator*= (Quaternion rhs) { *this = *this * rhs; return *this; }

    private:
        XMVECTOR q_;
    };
}
