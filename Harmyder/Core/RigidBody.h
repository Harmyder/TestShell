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

        void SetTransform(const Common::OrthogonalTransform& transform) { transform_ = transform; }
        const Common::OrthogonalTransform& GetTransform() { return transform_; }

    private:
        const PointCloudRigid& pointCloud_;
        Common::OrthogonalTransform transform_;
    };

    template <class BV>
    class RigidBodyBV : public RigidBody {
    public:
        RigidBodyBV(const PointCloudRigid& pointCloud, const BV& bv) : RigidBody(pointCloud), bv_(bv) {}
        ~RigidBodyBV() {}

        const BV& BoundingVolume() const { return bv_; }

    private:
        BV bv_;
    };
}
