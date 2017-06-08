#include "stdafx.h"
#include "HeatEquation2d.h"

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Pipeline/Helpers/CreateInputMesh.h"
#include "Viewer/Raii.h"
#include "Simulations/Utility.h"
using namespace Common;
using namespace Pipeline;
using namespace Viewer;
using namespace std;

#include "Common/Print/DebugPrint.h"

const int kMaxTemp = 1;
const float kPlateWidth = 2.f;
const float kPlateHeight = 2.f;
const uint32 kSurfaceWidth = 25;
const uint32 kSurfaceHeight = 25;

CONS_DEF(HeatEquation2d)
HeatEquation2d::~HeatEquation2d() {}

void HeatEquation2d::Init() {
    temperature_ = make_unique<Dynarray<Dynarray<float>>>(kSurfaceHeight, Dynarray<float>(kSurfaceWidth, 0.f));
    InitTemperature();

    auto inputMesh = make_unique<InputMesh>("splineSurface");
    Helpers::CreateGridXy(kSurfaceWidth, kSurfaceHeight, kPlateWidth, kPlateHeight, *inputMesh);
    inputScene_ = make_unique<InputScene>();
    inputScene_->AddMesh(move(inputMesh));

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    InitBlankPhysicsData();

    matPlate_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kWhite(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *matPlate_, *matPlate_);
    descs.Vertices[0].texture = ComposeTextureData(false);

    plate_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void HeatEquation2d::Step(float deltaTime) {
    InitTemperature();
    // TODO: Find out which step size we can use here to go faster
    float physicsDeltaTime = 0.002f;
    const int stepsCount = (int)(deltaTime / physicsDeltaTime);
    for (int i = 0; i < stepsCount * 10; ++i) StepPhysics(physicsDeltaTime);
    ComposeTextureData(true);
}

// Explicit FTCS method
void HeatEquation2d::StepPhysics(float dT) {
    auto& t = *temperature_.get();
    array<Dynarray<float>, 2> tmp;
    tmp[0] = t[0];
    int currentRow = 1;
    float absoluteChange = 0.f;
    for (uint32 i = 1; i < kSurfaceHeight - 1; ++i) {
        tmp[currentRow] = t[i];
        for (uint32 j = 1; j < kSurfaceWidth - 1; ++j) {
            auto xPart = (tmp[currentRow][j - 1] - 2 * tmp[currentRow][j] + tmp[currentRow][j + 1]) / (kPlateWidth * kPlateWidth);
            auto yPart = (t[i + 1][j] - 2 * tmp[currentRow][j] + tmp[1 - currentRow][j]) / (kPlateHeight * kPlateHeight);
            t[i][j] = t[i][j] + dT * (xPart + yPart);
            absoluteChange += abs(t[i][j] - tmp[currentRow][j]);
        }
        currentRow = 1 - currentRow;
    }
    DebugPrintf("%f\n", absoluteChange);
}

Texture HeatEquation2d::ComposeTextureData(bool forceRecreation) {
    const uint32 red = 0xFF0000FF;
    const uint32 blue = 0x00FFFF;
    Dynarray<uint32> texData(kSurfaceWidth * kSurfaceHeight);
    for (uint32 j = 0; j < kSurfaceHeight; ++j) {
        for (uint32 i = 0; i < kSurfaceWidth; ++i) {
            float t = (*temperature_)[i][j] / kMaxTemp;
            texData[j * kSurfaceWidth + i] = (255 << 24) + (uint8((1 - t) * 255) << 16) + (0 << 8) + uint8(t * 255);
        }
    }
    return viewport_.CreateTextureFromHandmadeData(L"temperature", kSurfaceWidth, kSurfaceHeight, ResourceFormat::kR8G8B8A8_UNORM(), texData.data(), forceRecreation);
}

void HeatEquation2d::InitTemperature() {
    const uint32 rangeI = kSurfaceHeight / 4;
    const uint32 rangeJ = kSurfaceWidth * 2 / 3;
    const uint32 offsetI = kSurfaceHeight * 2 / 3 - rangeI / 2;
    const uint32 offsetJ = kSurfaceWidth / 2 - rangeJ / 2;
    for (uint32 i = offsetI; i < offsetI + rangeI; ++i) {
        for (uint32 j = offsetJ; j < offsetJ + rangeJ; ++j) {
            (*temperature_)[i][j] = kMaxTemp;
        }
    }
}
