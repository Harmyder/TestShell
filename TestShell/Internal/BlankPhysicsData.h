#pragma once
#include "Pipeline/UserLevel/IPhysicsData.h"

class BlankPhysicsData : public Pipeline::IPhysicsData
{
public:
    BlankPhysicsData() : transform_(Common::kInfinity) {}

    Common::OrthogonalTransform GetTransform() override { return transform_; }
    void SetTransform(const Common::OrthogonalTransform& transform) override { transform_ = transform; }

    void GetPositions(std::vector<Common::Vector3>&) override { throw "BlankPhysicsData doesn't support this method"; }

private:
    Common::OrthogonalTransform transform_;
};
