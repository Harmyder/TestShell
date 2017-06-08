#pragma once

#include "Internal\BaseSimulation.h"
#include "Common\Math\Vector\Vector.h"

class MovingObjsInRi : public BaseSimulation
{
public:
    CONS_DEC(MovingObjsInRi);
    ~MovingObjsInRi();

public:
    void Init() override;
    void Step(float deltaTime) override;

    Common::Vector3 sun_pos;
    Common::Vector3 earth_pos;
    Common::Scalar velocity;
    float unused_time = 0.f;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> solsys_;
    std::unique_ptr<Viewer::MaterialRaii> matSun_;
    std::unique_ptr<Viewer::MaterialRaii> matEarth_;
};
