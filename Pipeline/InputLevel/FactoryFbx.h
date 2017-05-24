// **************************************************************************************
// Desc: Builds InputMesh
// **************************************************************************************
#pragma once

#include "ColliderType.h"

namespace FBX {
    struct Node;
}

namespace Common {
    class OrthogonalTransform;
}

namespace Pipeline
{
    class InputMesh;
    class InputCollider;

    struct ColliderFbx {
        std::string name;
        ColliderType type;
        float width = 0;
        float height = 0;
        float length = 0;
        float radius = 0;
    };

    class FactoryFbx
    {
    public:
        static std::unique_ptr<InputMesh> BuildMesh(const FBX::Node *node, const Common::OrthogonalTransform& globalTransform, float scaleFactor);
        static std::unique_ptr<InputCollider> BuildCollider(
            const FBX::Node *node,
            const Common::OrthogonalTransform& parentTransform,
            const Common::OrthogonalTransform& localTransform,
            float scaleFactor,
            const ColliderFbx& collider);
    };
}
