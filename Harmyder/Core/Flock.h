#pragma once
#include "Core\RigidBody.h"

namespace Harmyder
{
    template <class BV>
    class RigidBodyBV;
    class Mesh;

    struct FlockEntity {
        const std::shared_ptr<Mesh> mesh;
        uint32 count;
        float meanX;
        float stdDevX;
    };

    class Flock
    {
    public:
        Flock();

        static void Create(FlockEntity *types, uint32 typesCount, Flock *&flock);

    private:
        struct Piece {
            RigidBodyBV<Sphere> rb;
            const XMFLOAT3 scale;
        };
        std::vector<Piece> pieces_;

    };
}
