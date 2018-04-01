#pragma once

#include <btBulletDynamicsCommon.h>

#include "Common/Math/Vector/all.h"

namespace Exploring_Bullet
{
    inline btVector3& Tobt(Common::Vector3& v) { return *(btVector3*)&v; }
    inline const btVector3& Tobt(const Common::Vector3& v) { return *(btVector3*)&v; }

    inline btTransform& Tobt(Common::Matrix4& t) { return *(btTransform*)&t; }
    inline const btTransform& Tobt(const Common::Matrix4& t) { return *(btTransform*)&t; }

    inline const btTransform Tobt(const Common::OrthogonalTransform& t) { 
        auto affine = OrthoToAffine(t);
        btTransform res;
        static_assert(sizeof(btTransform) == sizeof(Common::AffineTransform), "");
        memcpy(&res, &affine, sizeof(btTransform));
        return res;
    }

    inline Common::Vector3& ToCommon(btVector3& t) { return *(Common::Vector3*)&t; }
    inline const Common::Vector3& ToCommon(const btVector3& t) { return *(Common::Vector3*)&t; }

    inline Common::Matrix4& ToCommon(btTransform& t) { return *(Common::Matrix4*)&t; }
    inline const Common::Matrix4& ToCommon(const btTransform& t) { return *(Common::Matrix4*)&t; }
}
