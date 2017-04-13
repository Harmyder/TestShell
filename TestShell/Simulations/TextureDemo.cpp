#include "stdafx.h"
#include "Simulations/TextureDemo.h"

#include <vector>
using namespace std;

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
using namespace Common;

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Pipeline/Helpers/CreateInputMesh.h"
using namespace Pipeline;

#include "Viewer/Vertex.h"
#include "Viewer/Raii.h"
using namespace Viewer;

#include "Simulations/Utility.h"

TextureDemo::TextureDemo(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("TextureDemo", viewport, gameInput) {}
TextureDemo::~TextureDemo() {}

void TextureDemo::Init() {
    auto inputMesh = make_unique<InputMesh>("cloth");
    Helpers::CreateGridXY(50, 25, 1.f, .5f, *inputMesh);
    inputScene_ = make_unique<InputScene>();
    inputScene_->AddMesh(move(inputMesh));

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    InitBlankPhysicsData();

    const wstring path = L"..\\..\\Assets\\Textures\\";
    const wstring filetitle = L"eric_cartman.png";
    viewport_.SetTextureRootDirectory(path);
    const auto texture = viewport_.CreateTextureFromFile(filetitle);

    mat_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kTurquesa(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *mat_, *mat_);
    descs.Vertices[0].texture = texture;

    sheet_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void TextureDemo::Step(float) {}
