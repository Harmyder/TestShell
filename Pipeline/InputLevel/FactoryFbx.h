// **************************************************************************************
// Desc: Builds InputMesh
// **************************************************************************************
#pragma once

#include "ColliderType.h"

namespace FBX
{
    struct Node;
};

namespace Pipeline
{
    class InputMesh;
    class InputCollider;

    struct ColliderFbx {
        std::string name;
        ColliderType type;
        float x = 0;
        float y = 0;
        float z = 0;
        float height = 0;
        float radius = 0;
    };

    class FactoryFbx
    {
    public:
        static std::unique_ptr<InputMesh> BuildMesh(const FBX::Node *node, const XMFLOAT4X4 &globalTransform, float scaleFactor);
        static std::unique_ptr<InputCollider> BuildCollider(const FBX::Node *node, const XMFLOAT4X4 &globalTransform, float scaleFactor, const ColliderFbx& collider);
    };
}
