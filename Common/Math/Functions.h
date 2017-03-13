#pragma once

#include "Math\Scalar.h"
#include "Math\Vector.h"
#include "Math\Matrix.h"

namespace Common {
    Scalar Length(Vector3 v) { return Scalar(XMVector3Length(v)); }
    Scalar LengthEst(Vector3 v) { return Scalar(XMVector3LengthEst(v)); }
    Scalar LengthSq(Vector3 v) { return Scalar(XMVector3LengthSq(v)); }
    Scalar Dot(Vector3 v1, Vector3 v2) { return Scalar(XMVector3Dot(v1, v2)); }
    Scalar Dot(Vector4 v1, Vector4 v2) { return Scalar(XMVector4Dot(v1, v2)); }
    Vector3 Cross(Vector3 v1, Vector3 v2) { return Vector3(XMVector3Cross(v1, v2)); }
    Vector3 Normalize(Vector3 v) { return Vector3(XMVector3Normalize(v)); }
    Vector4 Normalize(Vector4 v) { return Vector4(XMVector4Normalize(v)); }
}
