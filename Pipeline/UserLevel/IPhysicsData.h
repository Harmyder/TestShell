#pragma once

#include "Common/Math/Vector/Transform.h"

namespace Pipeline
{
    class IPhysicsData
    {
    public:
        virtual Common::Matrix4 GetTransform() = 0;
        virtual void SetTransform(const Common::Matrix4&) = 0;
    };
}
