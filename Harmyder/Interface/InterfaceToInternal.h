#pragma once
#include "Interface/HarmyderStructures.h"
#include "Interface/InternalHandle.h"
#include "Common/Math/Vector/Vector.h"
using namespace Common;

#include "Harmyder/SDK/GeometryTypes.h"
#include "Harmyder/Core/Flock.h"

Harmyder::Sphere ToSphere(const htSphere& sphere) {
    return Harmyder::Sphere(*(Vector3*)&sphere.center, Scalar(sphere.radius));
}

Harmyder::FlockEntity ToFlockEntity(const htFlockEntity& flockEntity) {
    return Harmyder::FlockEntity{
        static_cast<Harmyder::PointCloudRigidHandle>(flockEntity.pointCloud).GetValue(),
        ToSphere(flockEntity.sphere),
        { flockEntity.count[0], flockEntity.count[1], flockEntity.count[2] },
        flockEntity.meanX
    };
}
