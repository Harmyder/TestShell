#include "stdafx.h"
#include "BulletPhysicsData.h"

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "TypesConnector.h"

using namespace Common;

namespace Exploring_Bullet
{
    BulletPhysicsData::BulletPhysicsData(std::unique_ptr<btRigidBody> rigidBody) : rigidBody_(move(rigidBody)) {}
    BulletPhysicsData::~BulletPhysicsData() {}

    Matrix4 BulletPhysicsData::GetTransform() {
        return Matrix4(*(XMMATRIX*)&rigidBody_->getCenterOfMassTransform());
    }

    void BulletPhysicsData::SetTransform(const Common::Matrix4& transform) {
        rigidBody_->setWorldTransform(Tobt(transform));
    }
}
