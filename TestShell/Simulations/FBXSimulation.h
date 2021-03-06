#pragma once

#include "Internal\BaseSimulation.h"

class FbxSimulation : public BaseSimulation
{
public:
    CONS_DEC(FbxSimulation);
    ~FbxSimulation();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> sceneDescsVertices_;
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> sceneDescsTypes_;
    std::unique_ptr<Viewer::MaterialRaii> matRigid_;
    std::unique_ptr<Viewer::MaterialRaii> matCollider_;
};
