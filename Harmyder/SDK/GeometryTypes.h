#pragma once

namespace Harmyder
{
    struct Triangle
    {
        uint16 vertices[3];
    };

    struct Sphere
    {
        Sphere(Common::Vector3 center, Common::Scalar radius) : center(center), radius(radius) {}
        Sphere(const Sphere& other) : center(other.center), radius(other.radius) {}

        Common::Vector3 center;
        Common::Scalar radius;
    };
}
