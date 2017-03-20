#pragma once

#include "btBulletDynamicsCommon.h"

#include "Common/Math/Vector/all.h"

namespace Exploring_Bullet
{
    inline btVector3& Tobt(Common::Vector3& v) { return *(btVector3*)&v; }
    inline const btVector3& Tobt(const Common::Vector3& v) { return *(btVector3*)&v; }

    inline btTransform& Tobt(Common::AffineTransform& t) { return *(btTransform*)&t; }
    inline const btTransform& Tobt(const Common::AffineTransform& t) { return *(btTransform*)&t; }

    inline Common::AffineTransform& ToCommon(btTransform& t) { return *(Common::AffineTransform*)&t; }
    inline const Common::AffineTransform& ToCommon(const btTransform& t) { return *(Common::AffineTransform*)&t; }
}
