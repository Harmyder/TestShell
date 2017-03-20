#include "stdafx.h"

#include "UserLevel/UserMesh.h"
#include "InputLevel/InputMesh.h"

using namespace std;

namespace Pipeline
{
    UserMesh::UserMesh(const InputMesh &mesh) :
        NamedByRefObject(mesh.GetName()),
        input_(mesh),
        physicsData_(nullptr),
        transform_(Common::kIdentity)
    {}
}
