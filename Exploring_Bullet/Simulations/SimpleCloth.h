#pragma once
#include "BaseBulletSimulation.h"

class btSoftBodyRigidBodyCollisionConfiguration;
class btCollisionDispatcher;
class btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btSoftRigidDynamicsWorld;
class btSoftBodyWorldInfo;

namespace Exploring_Bullet
{
    class SimpleCloth : public BaseBulletSimulation
    {
    public:
        SimpleCloth(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
        ~SimpleCloth();

        void Init() override;
        void Step(float deltaTime) override;

    private:
        void CreateEmptyDynamicsWorld();

    private:
        std::unique_ptr<btSoftBodyRigidBodyCollisionConfiguration> collisionConfig_;
        std::unique_ptr<btCollisionDispatcher> dispatcher_;
        std::unique_ptr<btDbvtBroadphase> broadphase_;
        std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
        std::unique_ptr<btSoftRigidDynamicsWorld> dynamicsWorld_;
        std::unique_ptr<btSoftBodyWorldInfo> softBodyWorldInfo_;
        
        std::unique_ptr<Viewer::RenderItemOpaqueRaii> clothVisual_;
        std::unique_ptr<Pipeline::InputScene> inputScene_;
        Pipeline::UserMesh* clothMesh_;

        std::unique_ptr<Viewer::MaterialRaii> matCloth_;
    };
}
