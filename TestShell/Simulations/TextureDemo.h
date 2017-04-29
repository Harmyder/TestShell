#pragma once

#include "Internal\BaseSimulation.h"

class TextureDemo : public BaseSimulation
{
public:
    TextureDemo(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput);
    ~TextureDemo();

public:
    void Init() override;
    void Step(float deltaTime) override;

private:
    std::unique_ptr<Viewer::RenderItemOpaqueRaii> sheet_;
    std::unique_ptr<Viewer::MaterialRaii> mat_;

    std::unique_ptr<Pipeline::InputScene> inputScene_;
};
