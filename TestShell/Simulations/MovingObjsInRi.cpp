#include "stdafx.h"
#include "Simulations\MovingObjsInRi.h"

#include "Viewer\Viewport.h"
#include "Viewer\Raii.h"
#include "Common\Math\Vector\all.h"

using namespace Viewer;
using namespace Common;
using namespace std;

const Scalar kSunMass = 1.989f * 0.1e31f;
const Scalar kEarthMass = 5.972f * 0.1e25f;
const Scalar kVelocity = 30000;
const float kSunRadius = 695700;
const float kEarthRadius = 6371;
const float kRadius = 1.496e11f;
const Scalar kG = 6.67408e-11f;
const float kStep = 3600;

const Vector3 kSunPos(0.f, 0.f, 0.f);
const Vector3 kEarthPos(kRadius, 0.f, 0.f);
const Vector3 kNormal(kZUnit);
const float kDistScale = 10.f / kRadius;
const float kTimeScale = 1.f / (525600 * 6); // 1 minute = 1 year
const float kScaledStep = kStep * kTimeScale;
const float kSunVisualScale = 30000;
const float kEarthVisualScale = 300000;

auto acceleration(Scalar radius) { return kG * kSunMass / (radius * radius); }

CONS_DEF(MovingObjsInRi)
MovingObjsInRi::~MovingObjsInRi() {}

void MovingObjsInRi::Init() {
    sun_pos = kSunPos;
    earth_pos = kEarthPos;
    velocity = kVelocity;
        
    matSun_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kSilver(), "sun"));
    matEarth_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kBlue(), "earth"));

    const auto s = Matrix4::MakeTranslation(sun_pos) * Matrix4::MakeScale(kSunRadius * kDistScale * kSunVisualScale);
    const auto e = Matrix4::MakeTranslation(earth_pos * kDistScale) * Matrix4::MakeScale(kEarthRadius * kDistScale * kEarthVisualScale);
    const RenderItemTypeDesc sun("Sun", PredefinedGeometryType::kSphere, s.Store4x3(), *matSun_, PrimitiveTopology::kTriangleList());
    const RenderItemTypeDesc earth("Earth", PredefinedGeometryType::kSphere, e.Store4x3(), *matEarth_, PrimitiveTopology::kTriangleList());

    solsys_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque({ sun, earth }));
}

void MovingObjsInRi::Step(float deltaTime) {
    const float allTime = unused_time + deltaTime;
    const int stepsCount = (int)(allTime / kScaledStep);
    unused_time = allTime - kScaledStep * stepsCount;

    for (int i = 0; i < stepsCount; ++i) {
        const auto r = earth_pos - sun_pos;
        const auto rlen = Length(r);
        const auto vel = Cross(kNormal, r) / rlen * velocity;
        earth_pos += vel * kStep;
        velocity = Length(vel + Normalize(-(r + vel)) * acceleration(rlen));
    }
    const auto e = Matrix4::MakeTranslation(earth_pos * kDistScale) * Matrix4::MakeScale(kEarthRadius * kDistScale * kEarthVisualScale);
    viewport_.UpdateRenderSubItemTransform(*solsys_, "Earth", e.Store4x3());
}
