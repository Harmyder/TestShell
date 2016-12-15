// **************************************************************************************
// Desc: Builds InputMesh
// **************************************************************************************
#pragma once

namespace FBX
{
    struct Node;
};

namespace Pipeline
{
    class InputMesh;

    class FactoryFbx
    {
    public:
        void Build(InputMesh &mesh, const FBX::Node *node, const XMFLOAT4X4 &globalTransform, float scaleFactor);
    };
}
