#pragma once

#include "Interface\HarmyderHandle.h"
#include "Interface\HarmyderStructures.h"

void hfInit();
void hfClose();

hhPointCloudRigid hfPointCloudRigidCreate(const htPosition *points, uint32 pointsCount);
htSphere hfComputeSphereBV(hhPointCloudRigid pointCloud);
void hfPointCloudRigidDestroy(hhPointCloudRigid pointCloud);

hhMeshRigid hfMeshRigidCreate(const htPosition *vertices, uint32 verticesCount, const htTriangle *triangles, uint32 trianglesCount);
void hfMeshRigidDestroy(hhMeshRigid mesh);

hhRigidBody hfRigidBodyCreate(hhPointCloudRigid pointCloud, htSphere bv);
void hfRigidBodySetTransform(hhRigidBody rigidBody, const htTransform& transform);
const htTransform& hfRigidBodyGetTransform(hhRigidBody rigidBody);
void hfRigidBodyDestroy(hhRigidBody rigidBody);

hhFlock hfFlockCreate(const char* name, const htFlockEntity &type);
uint32 hfFlockGetPiecesCount(hhFlock flock);
const htFlockPiece* hfFlockGetPieces(hhFlock flock);
void hfFlockDestroy(hhFlock flock);

