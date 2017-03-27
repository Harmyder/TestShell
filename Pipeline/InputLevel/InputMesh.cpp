#include "stdafx.h"

#include "InputLevel/InputMesh.h"
using namespace std;

namespace Pipeline
{
    void InputMesh::SetTransform(const XMFLOAT4X4& transform) {
        assert((int)isInstanced_ & (int)InstanceOption::CanBeNotInstanced);
        isInstanced_ = InstanceOption::NotInstanced;
        transform_ = transform;
    }

    void InputMesh::SetTransforms(vector<XMFLOAT4X3>&& transform) {
        assert((int)isInstanced_ & (int)InstanceOption::CanBeInstanced);
        isInstanced_ = InstanceOption::Instanced;
        transforms_ = move(transform);
    }
}