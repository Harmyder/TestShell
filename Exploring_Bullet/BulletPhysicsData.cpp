#include "stdafx.h"
#include "BulletPhysicsData.h"

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "TypesConnector.h"

using namespace Common;

namespace Exploring_Bullet
{
    BulletPhysicsData::BulletPhysicsData(std::unique_ptr<btRigidBody> rigidBody) : rigidBody_(move(rigidBody)) {}
    BulletPhysicsData::~BulletPhysicsData() {}

    OrthogonalTransform BulletPhysicsData::GetTransform() {
        return OrthogonalTransform(*(XMMATRIX*)&rigidBody_->getCenterOfMassTransform());
    }

    void BulletPhysicsData::SetTransform(const Common::OrthogonalTransform& transform) {
        rigidBody_->setWorldTransform(Tobt(OrthoToAffine(transform)));
    }
}
