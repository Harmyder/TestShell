#include "stdafx.h"
#include "HarmyderInterface.h"

#include "HarmyderModule.h"
#include "Interface\InternalHandle.h"
#include "Interface\InterfaceToInternal.h"

#include "Core\RigidBody.h"
#include "Core\Mesh.h"
#include "Core\Flock.h"
#include "SDK\BoundingVolumes.h"

using namespace Harmyder;
using namespace std;
using namespace Common;

#pragma warning(disable: 4239)

void hfInit() {
    HarmyderModule::GetInstance().Init();
}

void hfClose() {
    HarmyderModule::GetInstance().Close();
}

hhPointCloudRigid hfPointCloudRigidCreate(const htPosition *points, uint32 pointsCount) {
    auto pointCloud = new PointCloudRigid((const Vector3*)points, pointsCount);
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
    auto mesh = new MeshRigid((Vector3*)vertices, verticesCount, (Triangle*)triangles, trianglesCount);
    return hhMeshRigid(mesh);
}

void hfMeshRigidDestroy(hhMeshRigid mesh) {
    MeshRigid* m = static_cast<MeshRigidHandle>(mesh).GetValue();
    delete m;
}

hhRigidBody hfRigidBodyCreate(hhPointCloudRigid pointCloud, htSphere bv) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    auto rb = new RigidBodyBV<Sphere>(*pc, ToSphere(bv));
    return hhRigidBody(rb);
}

void hfRigidBodySetTransform(hhRigidBody rigidBody, const htTransform& transform) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    ri->SetTransform(*(OrthogonalTransform*)&transform);
}

const htTransform& hfRigidBodyGetTransform(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    return *(htTransform*)&ri->GetTransform();
}

void hfRigidBodyDestroy(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    delete ri;
}

hhFlock hfFlockCreate(const char* name, const htFlockEntity &type) {
    unique_ptr<Flock> flock;
    Flock::Create(name, ToFlockEntity(type), flock);
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
