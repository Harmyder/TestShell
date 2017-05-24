#include "stdafx.h"
#include "BulletPhysicsData.h"

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletSoftBody/btSoftBody.h"
#include "TypesConnector.h"

using namespace Common;
using namespace std;

namespace Exploring_Bullet
{
    BulletRigidPhysicsData::BulletRigidPhysicsData(unique_ptr<btRigidBody> rigidBody) : rigidBody_(move(rigidBody)) {}
    BulletRigidPhysicsData::~BulletRigidPhysicsData() {}

    OrthogonalTransform BulletRigidPhysicsData::GetTransform() {
        return OrthogonalTransform(*(XMMATRIX*)&rigidBody_->getCenterOfMassTransform());
    }

    void BulletRigidPhysicsData::SetTransform(const OrthogonalTransform& transform) {
        rigidBody_->setWorldTransform(Tobt(transform));
    }


    BulletSoftPhysicsData::BulletSoftPhysicsData(unique_ptr<btSoftBody> softBody) : softBody_(move(softBody)) {}
    BulletSoftPhysicsData::~BulletSoftPhysicsData() {}

    OrthogonalTransform BulletSoftPhysicsData::GetTransform() {
        return OrthogonalTransform(*(XMMATRIX*)&softBody_->getWorldTransform());
    }

    void BulletSoftPhysicsData::SetTransform(const OrthogonalTransform& transform) {
        softBody_->setWorldTransform(Tobt(transform));
    }

    void BulletSoftPhysicsData::GetPositions(std::vector<Common::Vector3>& positions) {
        for (uint32 i = 0; i < (uint32)positions.size(); ++i) {
            positions[i] = ToCommon(softBody_->m_nodes[i].m_x);
        }
    }
}
