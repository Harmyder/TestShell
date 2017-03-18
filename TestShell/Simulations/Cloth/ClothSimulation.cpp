#include "stdafx.h"
#include "Simulations/Cloth/ClothSimulation.h"

#include <vector>
using namespace std;

#include "Common/Math/Vector/Vector.h"
#include "Common/Math/Vector/Matrix.h"
using namespace Common;

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
using namespace Pipeline;

#include "Viewer/Vertex.h"
using namespace Viewer;

#include "Simulations\Utility.h"

namespace
{
     void CreateGridXY(uint16 xCount, uint16 yCount, float width, float height, unique_ptr<InputMesh>& output) {
        vector<XMFLOAT3> grid(xCount * yCount);
        const Vector3 base(-width / 2.f, -height / 2.f, 0.f);
        const float stepX = width / xCount;
        const float stepY = height / yCount;
        for (uint32 i = 0; i < xCount; ++i) {
            for (uint32 j = 0; j < yCount; ++j) {
                grid[i * yCount + j] = Vector3(base + Vector3(i * stepX, j * stepY, 0.f)).Store();
            }
        }
        // 0--h+0--2h+0
        // |\ |  \ |
        // | \|   \|
        // 1--h+1--2h+1
        const uint16 trianglesCount = 2 * (xCount - 1) * (yCount - 1);
        vector<uint16> indices(trianglesCount * 3);
        uint16 cur = 0;
        for (uint16 i = 0; i < xCount - 1; ++i) {
            for (uint16 j = 0; j < yCount - 1; ++j) {
                indices[cur++] = j + i * yCount;
                indices[cur++] = j + (i + 1) * yCount;
                indices[cur++] = j + (i + 1) * yCount + 1;
                indices[cur++] = j + i * yCount;
                indices[cur++] = j + (i + 1) * yCount + 1;
                indices[cur++] = (j + 1) + i * yCount;
            }
        }
        vector<XMFLOAT3> normals(grid.size(), { 0.f, 0.f, 1.f });
        vector<XMFLOAT2> uvs(grid.size());
        for (uint16 i = 0; i < xCount; ++i) {
            for (uint16 j = 0; j < yCount; ++j) {
                uvs[i * xCount + j].x = float(i) / xCount;
                uvs[i * xCount + j].y = float(j) / yCount;
            }
        }
        auto indicesUvs = indices;
        output->SetPositions(grid);
        output->SetNormals(normals);
        output->SetTrianglesPositions(indices);
        output->SetTexCoords(uvs);
        output->SetTrianglesTexCoords(indicesUvs);
        output->ComputeVertices();
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

    auto descs = BuildDescsFromScene(*scene_);

    viewport_.CreateMaterial(Material::kTurquesa(), "rigid"); // Though it is deformable, but this comes from BuildDescsFromScene
    cloth_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void ClothSimulation::Step(float deltaTime) {
    deltaTime;
}

void ClothSimulation::Quit() {
    viewport_.DestroyRenderItemOpaque(*cloth_);
    viewport_.DestroyMaterial("rigid");
}