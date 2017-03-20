#pragma once
#include "Pipeline/UserLevel/IPhysicsData.h"

class BlankPhysicsData : public Pipeline::IPhysicsData
{
public:
    BlankPhysicsData() : transform_(Common::kInfinity) {}

    Common::OrthogonalTransform GetTransform() override { return transform_; }
    void SetTransform(const Common::OrthogonalTransform& transform) override { transform_ = transform; }

private:
    Common::OrthogonalTransform transform_;
};
