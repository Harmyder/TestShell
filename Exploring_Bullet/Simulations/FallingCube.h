#pragma once
#include "BaseBulletSimulation.h"

class btCollisionShape;
template <class T> class btAlignedObjectArray;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;

namespace Pipeline
{
    class InputScene;
    class UserMesh;
}
namespace Viewer
{
    struct StructRenderItemId;
}

namespace Exploring_Bullet
{
    class FallingCube : public BaseBulletSimulation
    {
    public:
        FallingCube(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
        ~FallingCube();

        void Init() override;
        void Step(float deltaTime) override;
        void Quit() override;

    private:
        auto FallingCube::CreateGround();

    private:
        std::unique_ptr<btAlignedObjectArray<btCollisionShape*>> collisionShapes_;
        std::unique_ptr<btBroadphaseInterface> broadphase_;
        std::unique_ptr<btCollisionDispatcher> dispatcher_;
        std::unique_ptr<btConstraintSolver>	solver_;
        std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration_;
        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld_;

        std::unique_ptr<Viewer::StructRenderItemId> world_;
        std::unique_ptr<Pipeline::InputScene> inputScene_;
        Pipeline::UserMesh* falling_;
    };
}
