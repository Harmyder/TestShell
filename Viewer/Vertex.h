#pragma once
#include "Common\DirectXInclude.h"

namespace Viewer
{
    struct VertexNormalTex
    {
        VertexNormalTex() {}
        VertexNormalTex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& texCoord) :
            Position(position), Normal(normal), TexCoord(texCoord) {}

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };

    struct VertexColor
    {
        VertexColor() {}
        VertexColor(const XMFLOAT3& position, const XMFLOAT4& color) : Position(position), Color(color) {}

        XMFLOAT3 Position;
        XMFLOAT4 Color;
    };

    struct VertexParticles
    {
        VertexParticles() {}
        VertexParticles(const XMFLOAT3& position) : Position(position) {}

        XMFLOAT3 Position;
    };
}