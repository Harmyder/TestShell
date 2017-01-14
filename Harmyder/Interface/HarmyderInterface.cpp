#include "stdafx.h"
#include "HarmyderInterface.h"

#include "HarmyderModule.h"
#include "Interface\InternalHandle.h"

#include "Core\RigidBody.h"
#include "Core\Mesh.h"
#include "Core\Flock.h"
#include "SDK\BoundingVolumes.h"

using namespace Harmyder;
using namespace std;

static_assert(sizeof(FlockEntity) == sizeof(htFlockEntity), "Keep FlockEntity and htFlockEntity in sync");
static_assert(sizeof(Sphere) == sizeof(htSphere), "Keep Sphere and htSphere in sync");

void hfInit() {
    HarmyderModule::GetInstance().Init();
}

void hfClose() {
    HarmyderModule::GetInstance().Close();
}

hhPointCloudRigid hfPointCloudRigidCreate(const htPosition *points, uint32 pointsCount) {
    auto pointCloud = new PointCloudRigid((const XMFLOAT3*)points, pointsCount);
    return hhPointCloudRigid(pointCloud);
}

htSphere hfComputeSphereBV(hhPointCloudRigid pointCloud) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    Sphere s = Compute(pc->Points(), pc->PointsCount());
    return *(htSphere*)&s;
}

void hfPointCloudRigidDestroy(hhPointCloudRigid pointCloud) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    delete pc;
}

hhMeshRigid hfMeshRigidCreate(const htPosition *vertices, uint32 verticesCount, const htTriangle *triangles, uint32 trianglesCount) {
    auto mesh = new MeshRigid((XMFLOAT3*)vertices, verticesCount, (Triangle*)triangles, trianglesCount);
    return hhMeshRigid(mesh);
}

void hfMeshRigidDestroy(hhMeshRigid mesh) {
    MeshRigid* m = static_cast<MeshRigidHandle>(mesh).GetValue();
    delete m;
}

hhRigidBody hfRigidBodyCreate(hhPointCloudRigid pointCloud, htSphere bv) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    auto rb = new RigidBodyBV<Sphere>(*pc, *(Sphere*)&bv);
    return hhRigidBody(rb);
}

void hfRigidBodySetTransform(hhRigidBody rigidBody, const htTransform4x4& transform) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    ri->SetTransform(*(DirectX::XMFLOAT4X4*)&transform);
}

const htTransform4x4& hfRigidBodyGetTransform(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    return *(htTransform4x4*)&ri->GetTransform();
}

void hfRigidBodyDestroy(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    delete ri;
}

hhFlock hfFlockCreate(const char* name, const htFlockEntity &type) {
    unique_ptr<Flock> flock;
    Flock::Create(name, *(FlockEntity*)&type, flock);
    return hhFlock(flock.release());
}

uint32 hfFlockGetPiecesCount(hhFlock flock) {
    Flock* f = static_cast<FlockHandle>(flock).GetValue();
    return f->GetPiecesCount();
}

const htFlockPiece* hfFlockGetPieces(hhFlock flock) {
    Flock* f = static_cast<FlockHandle>(flock).GetValue();
    return (const htFlockPiece*)f->GetPieces();
}

void hfFlockDestroy(hhFlock flock) {
    Flock* f = static_cast<FlockHandle>(flock).GetValue();
    delete f;
}
