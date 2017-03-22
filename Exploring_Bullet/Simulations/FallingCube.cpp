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
using namespace Pipeline;

#include "Viewer/Vertex.h"
using namespace Viewer;

#include "TestShell/Simulations/Utility.h"
#include "TypesConnector.h"
#include "BulletPhysicsData.h"

namespace
{
    void CreateBox(float x, float y, float z, unique_ptr<InputMesh>& output) {
        auto box = GeometryGenerator::CreateBox(x, y, z);

        auto v = GeometryGenerator::ComputeVertices(box.TrianglesPositions, box.TrianglesTexCoords);
        output->SetVisualVertices(move(v.UniqueVertices));
        output->SetTrianglesVertices(move(v.TrianglesVertices));

        output->SetPositions(box.Positions);
        output->SetNormals(box.Normals);
        output->SetTrianglesPositions(box.TrianglesPositions);
        output->SetTexCoords(box.TexCoords);
        output->SetTrianglesTexCoords(box.TrianglesTexCoords);
        output->SetTransform(Matrix4(kIdentity).Store4x4());
    }

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
    FallingCube::FallingCube(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
        BaseBulletSimulation("Bullet:FallingCube", viewport, gameInput) {}
    FallingCube::~FallingCube() {}

    auto FallingCube::CreateGround() {
        const float groundX = 50.f; const float groundY = 50.f; const float groundZ = 50.f;
        const Vector3 groundDims(groundX, groundY, groundZ);
        const Vector3 groundOrigin(0.f, -groundY, 0.f);
        btBoxShape* groundShape = new btBoxShape(Tobt(groundDims));
        collisionShapes_->push_back(groundShape);
        auto groundTransform = Matrix4::MakeTranslation(groundOrigin);
        auto groundRigidBody = CreateRigidBody(dynamicsWorld_.get(), groundShape, 0, Tobt(groundTransform));

        auto inputMesh = make_unique<InputMesh>("ground");
        CreateBox(groundX, groundY, groundZ, inputMesh);
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

        btBoxShape* colShape = new btBoxShape(btVector3(.1, .1, .1));
        //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        collisionShapes_->push_back(colShape);

        //constexpr int kSize = 5;
        //for (int k = 0; k < kSize; k++) {
        //    for (int i = 0; i < kSize; i++) {
        //        for (int j = 0; j < kSize; j++) {
        //            startTransform.setOrigin(btVector3(btScalar(0.2*i), btScalar(2 + .2*k), btScalar(0.2*j)));
        //
        //        }
        //    }
        //}

        const float fallingX = 5.f; const float fallingY = 5.f; const float fallingZ = 5.f;
        const Vector3 fallingDims(fallingX, fallingY, fallingZ);
        const Vector3 fallingOrigin(0.f, fallingY * 10.f, -10.f);
        btBoxShape* fallingShape = new btBoxShape(Tobt(fallingDims));
        collisionShapes_->push_back(fallingShape);
        auto fallingTransform = Matrix4::MakeTranslation(fallingOrigin);
        btScalar mass(1.f);
        auto fallingRigidBody = CreateRigidBody(dynamicsWorld_.get(), fallingShape, mass, Tobt(fallingTransform));

        auto inputMesh = make_unique<InputMesh>("falling");
        CreateBox(fallingX, fallingY, fallingZ, inputMesh);
        inputScene_->AddMesh(move(inputMesh));

        scene_ = make_unique<UserScene>();
        UserSceneFactory::BuildScene(*scene_, *inputScene_);

        scene_->GetMeshNonConst(scene_->SearchMesh("ground")).InitPhysicsData(make_unique<BulletPhysicsData>(move(groundRigidBody)));
        falling_ = &scene_->GetMeshNonConst(scene_->SearchMesh("falling"));
        falling_->InitPhysicsData(make_unique<BulletPhysicsData>(move(fallingRigidBody)));

        auto descs = BuildDescsFromScene(*scene_);

        viewport_.CreateMaterial(Material::kJade(), "rigid");
        world_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
    }

    void FallingCube::Step(float deltaTime) {
        dynamicsWorld_->stepSimulation(deltaTime);
        viewport_.UpdateRenderSubitemTransform(*world_, "falling", falling_->GetTransform().Store4x3());
    }

    void FallingCube::Quit() {
        viewport_.DestroyRenderItemOpaque(*world_);
        viewport_.DestroyMaterial("rigid");
    }
}