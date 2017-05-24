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
        virtual Common::OrthogonalTransform GetTransform() = 0;
        virtual void SetTransform(const Common::OrthogonalTransform&) = 0;

        virtual void GetPositions(std::vector<Common::Vector3>& positions) = 0;
    };
}
