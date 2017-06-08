#pragma once

#include "Internal\BaseSimulation.h"

namespace Common
{
    template <class T>
    class Dynarray;
}

namespace Viewer
{
    struct Texture;
}

class HeatEquation2d : public BaseSimulation
{
public:
    CONS_DEC(HeatEquation2d);
    ~HeatEquation2d();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    Viewer::Texture ComposeTextureData(bool forceRecreation);
    void StepPhysics(float dT);
    void InitTemperature();

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> plate_;
    std::unique_ptr<Viewer::MaterialRaii> matPlate_;

    std::unique_ptr<Pipeline::InputScene> inputScene_;

    std::unique_ptr<Common::Dynarray<Common::Dynarray<float>>> temperature_;
};

