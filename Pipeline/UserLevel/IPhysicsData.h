#pragma once

#include "Common/Math/Vector/Transform.h"

namespace Pipeline
{
    class IPhysicsData
    {
    public:
        virtual Common::OrthogonalTransform GetTransform() = 0;
        virtual void SetTransform(const Common::OrthogonalTransform&) = 0;
    };
}
