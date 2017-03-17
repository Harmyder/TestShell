#include "stdafx.h"
#include "Core\RigidBody.h"

#include "Core\Mesh.h"

using namespace std;
using namespace DirectX;
using namespace Common;

namespace Harmyder
{
    RigidBody::RigidBody(const PointCloudRigid& pointCloud) :
        pointCloud_(pointCloud),
        transform_(kIdentity)
    {}
}
