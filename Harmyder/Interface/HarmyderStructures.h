#pragma once

struct htPosition {
    float coordinates[3];
};

struct htTriangle {
    uint16 vertices[3];
};

struct htSphere {
    float radius;
    float center[3];
};

struct htTransform {
    float r0[4];
    float r1[4];
    float r2[4];
    float r3[4];
};

struct hhPointCloudRigid;
struct htFlockEntity {
    hhPointCloudRigid pointCloud;
    uint8 count[3];
    float meanX;
    // float stdDevX; -- not implemented, all of the same size for now
};
