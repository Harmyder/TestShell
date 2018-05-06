#pragma once

#include "Internal\BaseSimulation.h"

namespace Pipeline
{
    class InputScene;
    class UserMesh;
}

class Cloth;

class ClothSimulation : public BaseSimulation
{
public:
    CONS_DEC(ClothSimulation);
    ~ClothSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> clothRi_;
    std::unique_ptr<Viewer::MaterialRaii> matDeformable_;

    std::unique_ptr<Pipeline::InputScene> inputScene_;
    Pipeline::UserMesh* clothMesh_;
    std::unique_ptr<Cloth> cloth_;
};
