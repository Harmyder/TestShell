#pragma once
#include "BaseBulletSimulation.h"

class btCollisionShape;
template <class T> class btAlignedObjectArray;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;

namespace Common
{
    class Matrix4;
}

namespace Exploring_Bullet
{
    class FallingCube : public BaseBulletSimulation
    {
    public:
        BCONS_DEC(FallingCube);
        ~FallingCube();

        void Init() override;
        void Step(float deltaTime) override;

    private:
        auto CreateGround();

    private:
        std::unique_ptr<btAlignedObjectArray<btCollisionShape*>> collisionShapes_;
        std::unique_ptr<btBroadphaseInterface> broadphase_;
        std::unique_ptr<btCollisionDispatcher> dispatcher_;
        std::unique_ptr<btConstraintSolver>	solver_;
        std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration_;
        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld_;

        std::unique_ptr<Viewer::RenderItemOpaqueRaii> ground_;
        std::unique_ptr<Viewer::RenderItemWithInstancesRaii> falling_;
        std::unique_ptr<Pipeline::InputScene> inputScene_;

        std::unique_ptr<Viewer::MaterialRaii> matRigid_;
    };
}
