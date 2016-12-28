#pragma once
#include "Pile\DirectXInclude.h"

namespace Viewer
{
    struct Vertex
    {
        Vertex() {}
        Vertex(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT2 texCoord) : Position(position), Normal(normal), TexCoord(texCoord) {}

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };
}