#include "stdafx.h"
#include "Core\RigidBody.h"

#include "Core\Mesh.h"

using namespace std;
using namespace DirectX;

namespace Harmyder
{
    RigidBody::RigidBody(const PointCloudRigid& pointCloud) :
        pointCloud_(pointCloud)
    {
        XMStoreFloat4x4(&transform_, XMMatrixIdentity());
    }
}
