#include "stdafx.h"
#include "FallingCube.h"

using namespace std;

#include "btBulletDynamicsCommon.h"

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
using namespace Common;

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Pipeline/Helpers/CreateInputMesh.h"
using namespace Pipeline;

#include "Viewer/CameraController.h"
#include "Viewer/Vertex.h"
#include "Viewer/Raii.h"
using namespace Viewer;

#include "TestShell/Simulations/Utility.h"
#include "TypesConnector.h"
#include "BulletPhysicsData.h"

namespace
{
    auto CreateRigidBody(btDiscreteDynamicsWorld* world, btCollisionShape* shape, float mass, const btTransform& startTransform) {
        bool isDynamic = (mass != 0.f);
        btVector3 localInertia(0, 0, 0);
        if (isDynamic) shape->calculateLocalInertia(mass, localInertia);
        auto body = make_unique<btRigidBody>(mass, nullptr, shape, localInertia);
        body->setWorldTransform(startTransform);
        world->addRigidBody(body.get());
        return move(body);
    }
}

namespace Exploring_Bullet
{
    BCONS_DEF(FallingCube)
    FallingCube::~FallingCube() {}

    auto FallingCube::CreateGround() {
        const float groundX = 50.f; const float groundY = 1.f; const float groundZ = 50.f;
        const Vector3 groundDims(groundX, groundY, groundZ);
        const Vector3 groundOrigin(0.f, -groundY, 0.f);
        btBoxShape* groundShape = new btBoxShape(Tobt(groundDims));
        collisionShapes_->push_back(groundShape);
        auto groundTransform = Matrix4::MakeTranslation(groundOrigin);
        auto groundRigidBody = CreateRigidBody(dynamicsWorld_.get(), groundShape, 0, Tobt(groundTransform));

        auto inputMesh = make_unique<InputMesh>("ground");
        Helpers::CreateBox(groundX, groundY, groundZ, OrthogonalTransform(kIdentity), *inputMesh);
        inputScene_->AddMesh(move(inputMesh));
        return groundRigidBody;
    }

    void FallingCube::Init() {
        collisionConfiguration_ = make_unique<btDefaultCollisionConfiguration>();
        dispatcher_ = make_unique<btCollisionDispatcher>(collisionConfiguration_.get());
        broadphase_ = make_unique<btDbvtBroadphase>();
        solver_ = make_unique<btSequentialImpulseConstraintSolver>();
        dynamicsWorld_ = make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
        dynamicsWorld_->setGravity(btVector3(0, -10, 0));

        inputScene_ = make_unique<InputScene>();

        collisionShapes_ = make_unique<btAlignedObjectArray<btCollisionShape*>>();
        auto groundRigidBody = CreateGround();

        btBoxShape* colShape = new btBoxShape(btVector3(.1f, .1f, .1f));
        //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        collisionShapes_->push_back(colShape);

        constexpr int kSize = 5;
        vector<OrthogonalTransform> transforms;
        transforms.reserve(kSize * kSize * kSize);
        for (int k = 0; k < kSize; k++) {
            for (int i = 0; i < kSize; i++) {
                for (int j = 0; j < kSize; j++) {
                    transforms.push_back(OrthogonalTransform::MakeTranslation(.2f*i, 5.f + .2f*k, .2f*j));
                }
            }
        }

        const float fallingX = .2f; const float fallingY = .2f; const float fallingZ = .2f;

        auto inputMesh = make_unique<InputMesh>("falling");
        Helpers::CreateBoxInstanced(fallingX, fallingY, fallingZ, transforms, *inputMesh);
        inputScene_->AddMesh(move(inputMesh));

        scene_ = make_unique<UserScene>();
        UserSceneFactory::BuildScene(*scene_, *inputScene_);

        scene_->GetMeshNonConst(scene_->SearchMesh("ground")).InitPhysicsData(make_unique<BulletRigidPhysicsData>(move(groundRigidBody)));

        btBoxShape* fallingShape = new btBoxShape(btVector3(fallingX, fallingY, fallingZ));
        collisionShapes_->push_back(fallingShape);

        auto fallingMesh = &scene_->GetMeshNonConst(scene_->SearchMesh("falling"));
        auto inputTransforms = fallingMesh->GetInput().GetTransforms();
        const btScalar mass(1.f);
        vector<unique_ptr<BulletRigidPhysicsData>> fallingBodies;
        for (const auto& t : inputTransforms) {
            auto body = CreateRigidBody(dynamicsWorld_.get(), fallingShape, mass, Tobt(Matrix4(XMLoadFloat4x3(&t))));
            fallingBodies.push_back(make_unique<BulletRigidPhysicsData>(move(body)));
        }
        fallingMesh->InitPhysicsDatas(fallingBodies);

        matRigid_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kJade(), "rigid"));
        auto descs = BuildDescsFromScene(*scene_, *matRigid_, *matRigid_);

        ground_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
        falling_ = make_unique<RenderItemWithInstancesRaii>(viewport_.CreateRenderItemOpaqueWithInstances(descs.Instanced[0], sizeof(VertexNormalTex)));

        Vector3 eye(0., 15., 10.);
        cameraCtrl_.SetYaw(0.f);
        cameraCtrl_.SetPitch(1.f);
        cameraCtrl_.SetTranslation(Vector3(eye));
    }

    void FallingCube::Step(float deltaTime) {
        dynamicsWorld_->stepSimulation(deltaTime);
        vector<XMFLOAT4X3> transforms;
        auto& fallingMesh = scene_->GetMesh(scene_->SearchMesh("falling"));
        for (uint32 i = 0; i < fallingMesh.GetTransformsCount(); ++i) transforms.push_back(OrthoToAffine(fallingMesh.GetTransform(i)).Store4x3());
        viewport_.UpdateRenderWithInstancesTransforms(*falling_, Matrix4(kIdentity).Store4x3(), transforms.data());
    }
}