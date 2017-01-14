#include "stdafx.h"
#include "Core\Flock.h"

using namespace std;
using namespace DirectX;

namespace Harmyder
{
    void Flock::Create(const std::string& name, FlockEntity& type, unique_ptr<Flock>& flock) {
        struct FlockUniquePtrEnabler : public Flock { FlockUniquePtrEnabler(const std::string& name, const float scale) : Flock(name, scale) {} };
        flock = make_unique<FlockUniquePtrEnabler>(name, type.mean);
        flock->pointCloud_ = type.pointCloud;
        flock->sphereBV_ = type.sphereBV;
        flock->pieces_.resize(type.count[0] * type.count[1] * type.count[2]);
        const float r = flock->sphereBV_->radius;
        const float offsetX = 2 * r; 
        const float offsetY = r * tanf((float)M_PI / 3.f);  // triangle height
        const float offsetZ = r * 1.f / 3.f * sqrtf(6.f);   // tetrahedron height
        const float baseX = -offsetX * (type.count[0] - 1);
        const float baseY = -offsetY * (type.count[1] - 1);
        const float baseZ = -offsetZ * (type.count[2] - 1);
        XMMATRIX scale = XMMatrixScaling(flock->scale_, flock->scale_, flock->scale_);
        uint32 i = 0;
        for (uint32 x = 0; x < type.count[0]; ++x) {
            for (uint32 y = 0; y < type.count[1]; ++y) {
                for (uint32 z = 0; z < type.count[2]; ++z) {
                    XMMATRIX center = XMMatrixTranslation(baseX + x * offsetX, baseY + y * offsetY, baseZ + z * offsetZ);
                    center = XMMatrixMultiply(center, scale);
                    XMStoreFloat4x3(&flock->pieces_[i++].transform, center);
                }
            }
        }
    }
}