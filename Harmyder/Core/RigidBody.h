#pragma once

namespace Harmyder
{
    class PointCloudRigid;
    class MeshRigid;

    class RigidBody
    {
    public:
        RigidBody(const PointCloudRigid& pointCloud);
        virtual ~RigidBody() {}

        void SetTransform(const DirectX::XMFLOAT4X4& transform) { transform_ = transform; }
        const DirectX::XMFLOAT4X4& GetTransform() { return transform_; }

    private:
        const PointCloudRigid& pointCloud_;
        DirectX::XMFLOAT4X4 transform_;
    };

    template <class BV>
    class RigidBodyBV : public RigidBody {
    public:
        RigidBodyBV(const PointCloudRigid& pointCloud, BV bv) : RigidBody(pointCloud), bv_(bv) {}
        ~RigidBodyBV() {}

        const BV& BoundingVolume() const { return bv_; }

    private:
        BV bv_;
    };
}
