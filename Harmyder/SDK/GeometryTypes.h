#pragma once

namespace Harmyder
{
    struct Triangle
    {
        uint16 vertices[3];
    };

    struct Sphere
    {
        float radius;
        XMFLOAT3 center;
    };
}
