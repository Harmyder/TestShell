#pragma once
#include "Core\RigidBody.h"

namespace Harmyder
{
    class PointCloudRigid;

    struct FlockEntity {
        const PointCloudRigid* pointCloud;
        const Sphere* sphereBV;
        uint8 count[3];
        float mean;
        // float stdDevX; -- not implemented, for now all of the same size
    };

    struct Piece {
        XMFLOAT4X3 transform;
        //float scale; -- not implemented, all of the same size for now, use scale_ instead
    };

    class Flock
    {
    public:
        static void Create(const std::string& name, FlockEntity& type, std::unique_ptr<Flock>& flock);

        uint32 GetPiecesCount() { return (uint32)pieces_.size(); }
        const Piece* GetPieces() { return pieces_.data(); }

    private:
        Flock(const std::string& name, const float scale) : name_(name), scale_(scale) {}

    private:
        const std::string name_;
        const PointCloudRigid* pointCloud_;
        const Sphere* sphereBV_;

        const float scale_; // global scale for now
        std::vector<Piece> pieces_;
    };
}
