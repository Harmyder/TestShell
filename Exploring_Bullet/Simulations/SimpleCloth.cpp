#include "stdafx.h"
#include "SimpleCloth.h"

#include "btBulletDynamicsCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "TypesConnector.h"

#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/Helpers/CreateInputMesh.h"

#include "TestShell/Simulations/Utility.h"
#include "BulletPhysicsData.h"

#include "Viewer/Viewport.h"
#include "Viewer/Raii.h"
using namespace std;
using namespace Viewer;
using namespace Pipeline;

namespace Exploring_Bullet
{
    unique_ptr<btSoftBody> CreateSoftBody(btSoftBodyWorldInfo* softBodyWorldInfo, const UserMesh& userMesh, const vector<int>& attached) {
        const auto& mg = userMesh.GetGeometry();
        std::vector<float> masses(mg.UniquePositions.size(), 1.f);
        for (auto a : attached) masses[a] = 0;
        auto cloth = make_unique<btSoftBody>(softBodyWorldInfo, (int)mg.UniquePositions.size(), (const btVector3*)mg.UniquePositions.data(), masses.data());

        set<pair<int, int>> links;
        const auto appendLink = [&cloth, &links](int node1, int node2) {
            int first = min(node1, node2);
            int second = max(node1, node2);
            if (links.find(make_pair(first, second)) == links.end()) {
                links.insert(make_pair(first, second));
                cloth->appendLink(first, second);
            }
        };

        for (uint32 i = 0; i < (uint32)mg.TrianglesPositions.size() / 3; ++i) {
            const auto p0 = mg.TrianglesPositions[i * 3];
            const auto p1 = mg.TrianglesPositions[i * 3 + 1];
            const auto p2 = mg.TrianglesPositions[i * 3 + 2];
            cloth->appendFace(p0, p1, p2);
            appendLink(p0, p1);
            appendLink(p1, p2);
            appendLink(p2, p0);
        }
        cloth->getCollisionShape()->setMargin(0.001f);
        cloth->generateBendingConstraints(2, cloth->appendMaterial());
        cloth->setTotalMass(10);
        cloth->m_cfg.piterations = 5;
        cloth->m_cfg.kDP = 0.005f;

        return cloth;
    }

    void SimpleCloth::CreateEmptyDynamicsWorld() {
        collisionConfig_ = make_unique<btSoftBodyRigidBodyCollisionConfiguration>();
        dispatcher_ = make_unique<btCollisionDispatcher>(collisionConfig_.get());
        broadphase_ = make_unique<btDbvtBroadphase>();
        solver_ = make_unique<btSequentialImpulseConstraintSolver>();
        dynamicsWorld_ = make_unique<btSoftRigidDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfig_.get());
        dynamicsWorld_->setGravity(btVector3(0, -1, 0));
        softBodyWorldInfo_ = make_unique<btSoftBodyWorldInfo>();
        softBodyWorldInfo_->m_broadphase = broadphase_.get();
        softBodyWorldInfo_->m_dispatcher = dispatcher_.get();
        softBodyWorldInfo_->m_gravity = dynamicsWorld_->getGravity();
        softBodyWorldInfo_->m_sparsesdf.Initialize();
    }

    BCONS_DEF(SimpleCloth)
    SimpleCloth::~SimpleCloth() {}

    void SimpleCloth::Init() {
        CreateEmptyDynamicsWorld();

        auto inputMesh = make_unique<InputMesh>("cloth");
        const int sizeX = 50; const int sizeY = 50;
        Helpers::CreateGridXy(sizeX, sizeY, .5f, .5f, *inputMesh);
        inputScene_ = make_unique<InputScene>();
        inputScene_->AddMesh(move(inputMesh));

        scene_ = make_unique<UserScene>();
        UserSceneFactory::BuildScene(*scene_, *inputScene_);
        clothMesh_ = &scene_->GetMeshNonConst(scene_->SearchMesh("cloth"));
        
        auto clothBody = CreateSoftBody(softBodyWorldInfo_.get(), *clothMesh_, {0, 1, 2, sizeX - 3, sizeX -2, sizeX - 1});
        dynamicsWorld_->addSoftBody(clothBody.get());
        clothMesh_->InitPhysicsData(make_unique<BulletSoftPhysicsData>(move(clothBody)));

        matCloth_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kJade(), "rigid"));
        auto descs = BuildDescsFromScene(*scene_, *matCloth_, *matCloth_);

        clothVisual_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
    }

    void SimpleCloth::Step(float deltaTime) {
        dynamicsWorld_->stepSimulation(deltaTime);

        auto& mg = clothMesh_->GetGeometryNonConst();
        clothMesh_->GetPositions(mg.UniquePositions);
        assert(mg.TrianglesPositions.size() == mg.TrianglesVertices.size());
        for (uint32 i = 0; i < (uint32)mg.TrianglesVertices.size(); ++i) {
            mg.UniqueVertices[mg.TrianglesVertices[i]].Position = mg.UniquePositions[mg.TrianglesPositions[i]].Store();
        }
        viewport_.UpdateRenderSubItemVertexData(*clothVisual_, "cloth", (uint8*)mg.UniqueVertices.data());
    }
}
