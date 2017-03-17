#include "stdafx.h"
#include "Core\Flock.h"

using namespace std;
using namespace DirectX;
using namespace Common;

namespace Harmyder
{
    void Flock::Create(const std::string& name, FlockEntity& type, unique_ptr<Flock>& flock) {
        struct FlockUniquePtrEnabler : public Flock { 
            FlockUniquePtrEnabler(const std::string& name, const PointCloudRigid* pointCloud, const Sphere& bv, const float scale) : 
                Flock(name, pointCloud, bv, scale) {} 
        };
        flock = make_unique<FlockUniquePtrEnabler>(name, type.pointCloud, type.sphereBV, type.mean);
        flock->pieces_.resize(type.count[0] * type.count[1] * type.count[2], { Matrix4(kZero) });
        const float r = flock->sphereBV_.radius;
        const float offsetX = 2 * r; 
        const float offsetY = r * tanf((float)M_PI / 3.f);  // triangle height
        const float offsetZ = r * 1.f / 3.f * sqrtf(6.f);   // tetrahedron height
        const float baseX = -offsetX * (type.count[0] - 1);
        const float baseY = -offsetY * (type.count[1] - 1);
        const float baseZ = -offsetZ * (type.count[2] - 1);
        Matrix4 scale = Matrix4::MakeScale(flock->scale_);
        uint32 i = 0;
        for (uint32 x = 0; x < type.count[0]; ++x) {
            for (uint32 y = 0; y < type.count[1]; ++y) {
                for (uint32 z = 0; z < type.count[2]; ++z) {
                    Matrix4 center = Matrix4::MakeTranslation(baseX + x * offsetX, baseY + y * offsetY, baseZ + z * offsetZ);
                    flock->pieces_[i++].transform = center * scale;
                }
            }
        }
    }
}