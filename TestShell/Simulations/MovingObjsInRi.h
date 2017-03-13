#pragma once

#include "Internal\BaseSimulation.h"
#include "Common\Math\Vector.h"

namespace Viewer
{
    struct StructRenderItemId;
}

class MovingObjsInRi : public BaseSimulation
{
public:
    MovingObjsInRi(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~MovingObjsInRi();

public:
    void Init() override;
    void Step(float deltaTime) override;
    void Quit() override;

    Common::Vector3 sun_pos;
    Common::Vector3 earth_pos;
    Common::Scalar velocity;
    float unused_time = 0.f;

private:
    std::unique_ptr<Viewer::StructRenderItemId> solsys_;
};
