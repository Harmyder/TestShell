#pragma once
#include "Pipeline/UserLevel/IPhysicsData.h"

class BlankPhysicsData : public Pipeline::IPhysicsData
{
public:
    BlankPhysicsData() : transform_(Common::kInfinity) {}

    Common::Matrix4 GetTransform() override { return transform_; }
    void SetTransform(const Common::Matrix4& transform) override { transform_ = transform; }

private:
    Common::Matrix4 transform_;
};
