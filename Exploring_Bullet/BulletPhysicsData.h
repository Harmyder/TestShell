#pragma once

#include "Pipeline/UserLevel/IPhysicsData.h"
#include "Common/Attribute/NonCopyable.h"

class btRigidBody;

namespace Exploring_Bullet
{
    class BulletPhysicsData : public Pipeline::IPhysicsData, public Common::NonCopyable
    {
    public:
        BulletPhysicsData(std::unique_ptr<btRigidBody> rigidBody);
        ~BulletPhysicsData();

        Common::OrthogonalTransform GetTransform() override;
        void SetTransform(const Common::OrthogonalTransform&) override;

    private:
        std::unique_ptr<btRigidBody> rigidBody_;
    };
}
