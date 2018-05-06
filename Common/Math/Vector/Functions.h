#pragma once

#include "Math\Vector\Scalar.h"
#include "Math\Vector\Vector.h"
#include "Math\Vector\Matrix.h"

namespace Common {
    inline Scalar SqrtEst(Scalar s) { return Scalar(XMVectorSqrtEst(s)); }
    inline Scalar Length(Vector3 v) { return Scalar(XMVector3Length(v)); }
    inline Scalar LengthEst(Vector3 v) { return Scalar(XMVector3LengthEst(v)); }
    inline Scalar LengthSq(Vector3 v) { return Scalar(XMVector3LengthSq(v)); }
    inline Scalar Dot(Vector3 v1, Vector3 v2) { return Scalar(XMVector3Dot(v1, v2)); }
    inline Scalar Dot(Vector4 v1, Vector4 v2) { return Scalar(XMVector4Dot(v1, v2)); }
    inline Vector3 Cross(Vector3 v1, Vector3 v2) { return Vector3(XMVector3Cross(v1, v2)); }
    inline Vector3 Normalize(Vector3 v) { return Vector3(XMVector3Normalize(v)); }
    inline Vector4 Normalize(Vector4 v) { return Vector4(XMVector4Normalize(v)); }
}
