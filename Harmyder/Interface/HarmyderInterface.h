#pragma once

#include "Interface\HarmyderHandle.h"
#include "Interface\HarmyderStructures.h"

void hfInit();
void hfClose();

hhPointCloudRigid hfCreatePointCloudRigid(const htPosition *points, uint32 pointsCount);
htSphere hfComputeSphereBV(hhPointCloudRigid pointCloud);
void hfDestroyPointCloud(hhPointCloudRigid pointCloud);

hhMeshRigid hfCreateMeshRigid(const htPosition *vertices, uint32 verticesCount, const htTriangle *triangles, uint32 trianglesCount);
void hfDestroyMesh(hhMeshRigid mesh);

hhRigidBody hfCreateRigidBody(hhPointCloudRigid pointCloud, htSphere bv);
void hfSetTransform(hhRigidBody rigidBody, const htTransform& transform);
const htTransform& hfGetTransform(hhRigidBody rigidBody);
void hfDestroyRigidBody(hhRigidBody rigidBody);

hhFlock hfCreateFlock(const char* name);
void hfPopulateFlock(htFlockEntity *types, uint32 typesCount, float density);
void hfDestroyFlock(hhFlock flock);

