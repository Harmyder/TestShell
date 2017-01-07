#pragma once
#include "Interface\HarmyderHandle.h"

namespace Harmyder
{
    class RigidBody;
    class Flock;
    class PointCloudRigid;
    class MeshRigid;

    template <class C, class H>
    struct InternalHandle : H {
        InternalHandle(H ri) : H(ri) {}
        C* GetValue() { return static_cast<C*>(handle_); }
    };

    struct PointCloudRigidHandle : InternalHandle<PointCloudRigid, hhPointCloudRigid> { using InternalHandle::InternalHandle; };
    struct MeshRigidHandle : InternalHandle<MeshRigid, hhMeshRigid> { using InternalHandle::InternalHandle; };

    struct RigidBodyHandle : InternalHandle<RigidBody, hhRigidBody> { using InternalHandle::InternalHandle; };

    struct FlockHandle : InternalHandle<Flock, hhFlock> { using InternalHandle::InternalHandle; };

}
