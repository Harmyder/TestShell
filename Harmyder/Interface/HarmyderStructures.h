#pragma once

__declspec(align(16)) struct htPosition {
    float coordinates[4];
};

struct htTriangle {
    uint16 vertices[3];
};

struct htSphere {
    htPosition center;
    float radius;
};

__declspec(align(16)) struct htTransform {
    float r0[4];
    float r1[4];
    float r2[4];
    float r3[4];
};

struct hhPointCloudRigid;
struct htFlockEntity {
    hhPointCloudRigid pointCloud;
    htSphere sphere;
    uint8 count[3];
    float meanX;
    // float stdDevX; -- not implemented, all of the same size for now
};

struct htFlockPiece {
    htTransform transform;
};