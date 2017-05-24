#pragma once

#include "Pipeline/UserLevel/IPhysicsData.h"
#include "Common/Attribute/NonCopyable.h"

class btRigidBody;
class btSoftBody;

namespace Exploring_Bullet
{
    class BulletRigidPhysicsData : public Pipeline::IPhysicsData, public Common::NonCopyable
    {
    public:
        BulletRigidPhysicsData(std::unique_ptr<btRigidBody> rigidBody);
        ~BulletRigidPhysicsData();

        Common::OrthogonalTransform GetTransform() override;
        void SetTransform(const Common::OrthogonalTransform&) override;

        void GetPositions(std::vector<Common::Vector3>& positions) override { throw "You don't need vertices from btRigidBody"; }

    private:
        std::unique_ptr<btRigidBody> rigidBody_;
    };

    class BulletSoftPhysicsData : public Pipeline::IPhysicsData, public Common::NonCopyable
    {
    public:
        BulletSoftPhysicsData(std::unique_ptr<btSoftBody> softBody);
        ~BulletSoftPhysicsData();

        Common::OrthogonalTransform GetTransform() override;
        void SetTransform(const Common::OrthogonalTransform&) override;

        void GetPositions(std::vector<Common::Vector3>& positions) override;

    private:
        std::unique_ptr<btSoftBody> softBody_;
    };
}
