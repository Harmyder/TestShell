#include "stdafx.h"
#include "Simulations/TextureDemo.h"

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

namespace Spline
{
    // B-spline base functions calculation
    // Efficient version with memoization and exploiting of triangular structure for nonzero basis elements
    class BSplineBaseFuncs
    {
    public:
        BSplineBaseFuncs(Dynarray<float>&& knots, uint32 degree) : knots_(move(knots)), degree_(degree) {}

        const auto& GetKnots() const { return knots_; }
        auto GetDegree() const { return degree_; }
        auto GetKnotsExtent() const { return knots_.back() - knots_.front(); }

        // expects knot vector for clamped spline, so first and last(degree + 1) elems are zeros
        uint32 FindSpan(float t) const {
            // span[u_i, u_{ i + 1 }] never riches its end, so for the last span we need to also handle u_{ i + 1 }
            if (t == knots_.back()) return (uint32)knots_.size() - degree_ - 2;
            auto spanBack = upper_bound(knots_.cbegin() + degree_, knots_.cend() - degree_, t) - 1;
            return (uint32)distance(knots_.cbegin(), spanBack);
        }

        auto ComputeBase(uint32 spanBack, float t) const {
            Dynarray<float> bases(degree_ + 1, 0.f);
            bases[degree_] = 1.;
            array<float, 2> tcb = { 0, 0 }; // two current bases from the next lower level
            for (uint32 level = 2; level < degree_ + 2; ++level) {
                for (uint32 minus = 0; minus < level; ++minus) {
                    swap(tcb[0], tcb[1]);
                    tcb[1] = bases[degree_ - minus];
                    auto i = spanBack - minus;
                    auto left_denom = knots_[i + level - 1] - knots_[i];
                    auto left_numer = t - knots_[i];
                    auto left = tcb[1] == 0 ? 0 : tcb[1] * left_numer / left_denom;
                    auto right_denom = knots_[i + level] - knots_[i + 1];
                    auto right_numer = knots_[i + level] - t;
                    auto right = tcb[0] == 0 ? 0 : tcb[0] * right_numer / right_denom;
                    bases[degree_ - minus] = left + right;
                }
            }
            return bases;
        }

    private:
        const Dynarray<float> knots_;
        const uint32 degree_;
    };

    class BSpline : public BSplineBaseFuncs {
    public:
        BSpline(Dynarray<float>&& knots, Dynarray<Vector3>&& points, uint32 degree) : BSplineBaseFuncs(move(knots), degree), points_(move(points)) {}

        const auto& GetPoints() const { return points_; }

        auto ComputePoint(float t) const {
            auto spanBack = FindSpan(t);
            auto bases = ComputeBase(spanBack, t);
            Vector3 res(0.f);
            auto start = spanBack - GetDegree();
            for (uint32 i = start; i < start + GetDegree() + 1; ++i) {
                const auto& point = points_[i];
                res += bases[i - start] * point;
            }
            return res;
        }

    private:
        const Dynarray<Vector3> points_;
    };

    class BSplineSurface {
    public:
        BSplineSurface(Dynarray<float>&& uKnots, Dynarray<float>&& vKnots, Dynarray<Vector3>&& points, uint32 degree) :
            uBasis_(move(uKnots), degree),
            vBasis_(move(vKnots), degree),
            points_(move(points)),
            degree_(degree)
        {
            assert(points_.size() == GetUPointsCount() * GetVPointsCount());
        }

        auto GetUKnotsExtent() const { return uBasis_.GetKnotsExtent(); }
        auto GetVKnotsExtent() const { return vBasis_.GetKnotsExtent(); }
        auto GetUKnotsBegin() const { return uBasis_.GetKnots().front(); }
        auto GetVKnotsBegin() const { return vBasis_.GetKnots().front(); }

        auto ComputePoint(float u, float v) const {
            const auto uSpanBack = uBasis_.FindSpan(u);
            const auto uBases = uBasis_.ComputeBase(uSpanBack, u);
            const auto vSpanBack = vBasis_.FindSpan(v);
            const auto vBases = vBasis_.ComputeBase(vSpanBack, v);
            Vector3 res(0.f);
            const auto uStart = uSpanBack - degree_;
            const auto vStart = vSpanBack - degree_;
            for (uint32 j = vStart; j < vStart + degree_ + 1; ++j) {
                for (uint32 i = uStart; i < uStart + degree_ + 1; ++i) {
                    const auto& point = points_[j * GetUPointsCount() + i];
                    res += uBases[i - uStart] * vBases[j - vStart] * point;
                }
            }
            return res;
        }

    private:
        uint32 GetUPointsCount() const { return (uint32)uBasis_.GetKnots().size() - degree_ - 1; }
        uint32 GetVPointsCount() const { return (uint32)vBasis_.GetKnots().size() - degree_ - 1; }

    private:
        const BSplineBaseFuncs uBasis_;
        const BSplineBaseFuncs vBasis_;
        const Dynarray<Vector3> points_;
        const uint32 degree_;
    };
}

namespace
{
    auto fill(const Spline::BSplineSurface& evaluator, uint32 uCount, uint32 vCount) {
        const auto uBegin = evaluator.GetUKnotsBegin();
        const auto vBegin = evaluator.GetVKnotsBegin();
        const auto uStep = evaluator.GetUKnotsExtent() / (uCount - 1);
        const auto vStep = evaluator.GetVKnotsExtent() / (vCount - 1);
        Dynarray<Vector3> points(uCount * vCount);
        for (uint32 j = 0; j < vCount; ++j) {
            for (uint32 i = 0; i < uCount; ++i) {
                const auto u = uBegin + i * uStep;
                const auto v = vBegin + j * vStep;
                points[j * uCount + i] = evaluator.ComputePoint(u, v);
            }
        }
        return points;
    }

    auto hillsData(Dynarray<float>& uKnots, Dynarray<float>& vKnots, Dynarray<Vector3>& points, uint32& degree) {
        uKnots = Dynarray<float>({ 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4 });
        vKnots = Dynarray<float>({ 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4 });;
        const uint32 side = 7;
        points = Dynarray<Vector3>(side * side);
        for (uint32 j = 0; j < side; ++j) {
            for (uint32 i = 0; i < side; ++i) {
                points[j * side + i] = Vector3((float)i, (float)j, 0.f);
            }
        }
        points[1 * side + side / 2].SetZ(3.f);
        points[(side - 2) * side + side / 2].SetZ(3.f);
        points[side / 2 * side + 1].SetZ(3.f);
        points[side / 2 * side + side - 2].SetZ(3.f);
        degree = 3;
    }
}

TextureDemo::TextureDemo(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("TextureDemo", viewport, gameInput) {}
TextureDemo::~TextureDemo() {}

void TextureDemo::Init() {
    inputScene_ = make_unique<InputScene>();

    Texture texCartman, texHeights;

    {
        auto inputMesh = make_unique<InputMesh>("cartman");
        Helpers::CreateGridXy(50, 25, 1.f, .5f, *inputMesh);
        inputMesh->SetTransform(OrthogonalTransform::MakeTranslation(Vector3(0.f, 2.f, 0.f)));
        inputScene_->AddMesh(move(inputMesh));

        const wstring path = L"..\\..\\Assets\\Textures\\";
        const wstring filetitle = L"eric_cartman.png";
        viewport_.SetTextureRootDirectory(path);
        texCartman = viewport_.CreateTextureFromFile(filetitle);
    }

    const uint32 surfaceWidth = 50;
    const uint32 surfaceHeight = 100;
    {
        Dynarray<float> uKnots(surfaceWidth), vKnots(surfaceHeight);
        Dynarray<Vector3> points(surfaceWidth * surfaceHeight);
        uint32 degree;
        hillsData(uKnots, vKnots, points, degree);
        Spline::BSplineSurface surface(move(uKnots), move(vKnots), move(points), degree);
        const auto vertices = fill(surface, surfaceWidth, surfaceHeight);

        Dynarray<float> heights(vertices.size());
        for (int i = 0; i < (int)vertices.size(); ++i) heights[i] = vertices[i].GetZ();

        auto inputMesh = make_unique<InputMesh>("splineSurface");
        Helpers::CreateGridXy(surfaceWidth, surfaceHeight, 2.f, 2.f, *inputMesh, [&heights, surfaceWidth](int i, int j) { return heights[j * surfaceWidth + i]; });
        inputScene_->AddMesh(move(inputMesh));

        const auto maxHeight = *max_element(cbegin(heights), cend(heights));
        Dynarray<uint32> texData(surfaceWidth * surfaceHeight);
        for (uint32 j = 0; j < surfaceHeight; ++j) {
            for (uint32 i = 0; i < surfaceWidth; ++i) {
                const float height = heights[j * surfaceWidth + i] / maxHeight;
                const uint8 color = (uint8)(255 * height);
                texData[j * surfaceWidth + i] = (color << 24) + (color << 16) + (color << 8) + 255;
            }
        }
        texHeights = viewport_.CreateTextureFromHandmadeData(L"heights", surfaceWidth, surfaceHeight, ResourceFormat::kR8G8B8A8_UNORM(), texData.data(), false);
    }

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    InitBlankPhysicsData();

    mat_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kTurquesa(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *mat_, *mat_);
    descs.Vertices[0].texture = texCartman;
    descs.Vertices[1].texture = texHeights;

    sheet_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void TextureDemo::Step(float) {}
