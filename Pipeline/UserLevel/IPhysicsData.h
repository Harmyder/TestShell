#pragma once

#include "Common/Math/Vector/Transform.h"

namespace Common {
    class Vector3;
}

namespace Pipeline
{
    class IPhysicsData
    {
    public:
        virtual Common::Matrix4 GetTransform() = 0;
        virtual void SetTransform(const Common::Matrix4&) = 0;

        virtual void GetPositions(std::vector<Common::Vector3>& positions) = 0;
    };
}
