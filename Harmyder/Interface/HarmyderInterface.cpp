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

void hfInit() {
    HarmyderModule::GetInstance().Init();
}

void hfClose() {
    HarmyderModule::GetInstance().Close();
}

hhPointCloudRigid hfCreatePointCloudRigid(const htPosition *points, uint32 pointsCount) {
    auto pointCloud = new PointCloudRigid((const XMFLOAT3*)points, pointsCount);
    return hhPointCloudRigid(pointCloud);
}

htSphere hfComputeSphereBV(hhPointCloudRigid pointCloud) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    Sphere s = Compute(pc->Points(), pc->PointsCount());
    return *(htSphere*)&s;
}

void hfDestroyPointCloud(hhPointCloudRigid pointCloud) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    delete pc;
}

hhMeshRigid hfCreateMeshRigid(const htPosition *vertices, uint32 verticesCount, const htTriangle *triangles, uint32 trianglesCount) {
    auto mesh = new MeshRigid((XMFLOAT3*)vertices, verticesCount, (Triangle*)triangles, trianglesCount);
    return hhMeshRigid(mesh);
}

void hfDestroyMesh(hhMeshRigid mesh) {
    MeshRigid* m = static_cast<MeshRigidHandle>(mesh).GetValue();
    delete m;
}

hhRigidBody hfCreateRigidBody(hhPointCloudRigid pointCloud, htSphere bv) {
    PointCloudRigid* pc = static_cast<PointCloudRigidHandle>(pointCloud).GetValue();
    auto rb = new RigidBodyBV<Sphere>(*pc, *(Sphere*)&bv);
    return hhRigidBody(rb);
}

void hfSetTransform(hhRigidBody rigidBody, const htTransform& transform) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    ri->SetTransform(*(DirectX::XMFLOAT4X4*)&transform);
}

const htTransform& hfGetTransform(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    return *(htTransform*)&ri->GetTransform();
}

void hfDestroyRigidBody(hhRigidBody rigidBody) {
    RigidBody* ri = static_cast<RigidBodyHandle>(rigidBody).GetValue();
    delete ri;
}

hhFlock hfCreateFlock(const char* name, const htFlockEntity &type) {
    unique_ptr<Flock> flock;
    Flock::Create(name, *(FlockEntity*)&type, flock);
    return hhFlock(flock.release());
}

void hfDestroyFlock(hhFlock flock) {
    Flock* ri = static_cast<FlockHandle>(flock).GetValue();
    delete ri;
}