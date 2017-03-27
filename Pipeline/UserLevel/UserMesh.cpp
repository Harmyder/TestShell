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

    void UserMesh::InitPhysicsData(std::unique_ptr<IPhysicsData> physicsData) {
        assert(physicsData_ == nullptr);
        assert(isInstanced_ == InputMesh::InstanceOption::None);
        isInstanced_ = InputMesh::InstanceOption::NotInstanced;
        physicsData_ = move(physicsData);
    }

    const uint32 UserMesh::GetTransformsCount() const {
        if (isInstanced_ == InputMesh::InstanceOption::NotInstanced) return 1;
        else return (uint32)physicsDatas_.size();
    }
}
