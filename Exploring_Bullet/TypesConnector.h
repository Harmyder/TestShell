#pragma once

#include "btBulletDynamicsCommon.h"

#include "Common/Math/Vector/all.h"

namespace Exploring_Bullet
{
    inline btVector3& Tobt(Common::Vector3& v) { return *(btVector3*)&v; }
    inline const btVector3& Tobt(const Common::Vector3& v) { return *(btVector3*)&v; }

    inline btTransform& Tobt(Common::Matrix4& t) { return *(btTransform*)&t; }
    inline const btTransform& Tobt(const Common::Matrix4& t) { return *(btTransform*)&t; }

    inline Common::Matrix4& ToCommon(btTransform& t) { return *(Common::Matrix4*)&t; }
    inline const Common::Matrix4& ToCommon(const btTransform& t) { return *(Common::Matrix4*)&t; }
}
