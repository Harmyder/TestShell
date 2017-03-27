#include "stdafx.h"
#include "Simulations/Cloth/ClothSimulation.h"

#include <vector>
using namespace std;

#include "Common/Math/Vector/Vector.h"
#include "Common/Math/Vector/Matrix.h"
#include "Common/Geometry/GeometryGenerator.h"
using namespace Common;

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
using namespace Pipeline;

#include "Viewer/Vertex.h"
#include "Viewer/Raii.h"
using namespace Viewer;

#include "Simulations\Utility.h"

namespace
{
     void CreateGridXY(uint16 xCount, uint16 yCount, float width, float height, unique_ptr<InputMesh>& output) {
        auto grid = GeometryGenerator::CreateGridXY(xCount, yCount, width, height);

        auto v = GeometryGenerator::ComputeVertices(grid.TrianglesPositions, grid.TrianglesTexCoords);
        output->SetVisualVertices(move(v.UniqueVertices));
        output->SetTrianglesVertices(move(v.TrianglesVertices));

        output->SetPositions(grid.Positions);
        output->SetNormals(grid.Normals);
        output->SetTrianglesPositions(grid.TrianglesPositions);
        output->SetTexCoords(grid.TexCoords);
        output->SetTrianglesTexCoords(grid.TrianglesTexCoords);
        output->SetTransform(Matrix4(kIdentity).Store4x4());
    }
}

ClothSimulation::ClothSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("Clothsimulation", viewport, gameInput) {}
ClothSimulation::~ClothSimulation() {}

void ClothSimulation::Init() {
    auto inputMesh = make_unique<InputMesh>("cloth");
    CreateGridXY(25, 25, .5f, .5f, inputMesh);
    inputScene_ = make_unique<InputScene>();
    inputScene_->AddMesh(move(inputMesh));

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    InitBlankPhysicsData();

    matDeformable_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kTurquesa(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *matDeformable_, *matDeformable_);

    cloth_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void ClothSimulation::Step(float deltaTime) {
    deltaTime;
}

void ClothSimulation::Quit() {
    viewport_.DestroyRenderItemOpaque(*cloth_);
}