#pragma once


struct hhHandle {
    hhHandle(void *handle) : handle_(handle) {}

protected:
    void *handle_;
};

struct hhPointCloudRigid : hhHandle { hhPointCloudRigid(void *pointCloud) : hhHandle(pointCloud) {} };
struct hhMeshRigid : hhHandle { hhMeshRigid(void *mesh) : hhHandle(mesh) {} };

struct hhRigidBody : hhHandle { hhRigidBody(void *rigidBody) : hhHandle(rigidBody) {} };

struct hhFlock : hhHandle { hhFlock(void *flock) : hhHandle(flock) {} };
