#include "stdafx.h"
#include "Simulations/FEM/Displacement1d.h"

#include "Viewer/Viewport.h"
#include "Viewer/Raii.h"
#include "Common/Math/Vector/all.h"

// Here i want to implement simple example of FEM

using namespace Viewer;
using namespace std;
using namespace Common;

static const float bl = 0.f;
static const float br = .1f;
static const float bcl = 0.f;
static const float bcr = .001f;
static const float E = (float)1e11;

float basisFunc(float xi, int localNode) {
    if (localNode == 0) return (1 - xi) / 2.f;
    if (localNode == 1) return (1 + xi) / 2.f;
    throw;
}

float basisFuncGrad(float xi, int localNode) {
    UNREFERENCED_PARAMETER(xi);
    if (localNode == 0) return -1 / 2.;
    if (localNode == 1) return 1 / 2.;
    throw;
}

Displacement1d::Displacement1d(Viewport& viewport, const Viewer::GameInput& gameInput) : BaseSimulation("Displacement field 1d", viewport, gameInput) {
    force_ = [](float x) { return (float)1e11 * x; };

    const int kElemsCount = 10;
    const float kElemLength = (br - bl) / kElemsCount;
    std::vector<float> points(kElemsCount + 1);
    for (int i = 0; i < kElemsCount + 1; ++i) points.push_back(bl + kElemLength * i);

    std::vector<float> dofs(kElemsCount + 1);

}

Displacement1d::~Displacement1d() {}

void Displacement1d::Init() {}
void Displacement1d::Step(float) {}
