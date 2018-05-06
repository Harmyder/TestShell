#include "stdafx.h"
#include "Simulations/Cloth/ClothSimulation.h"

#include <vector>
#include <stdexcept>
using namespace std;

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
#include "Hashes.h"
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

#include "Simulations\Utility.h"

enum class ConstraintType {
    kStructural = 0,
    kShear = 1,
    kBending = 2,
    kConstraintTypesCount = 3,
};

struct Constraint {
    int v1, v2;
    float restingDistSq;
};

const Vector3 kGravityForce = {0.f, 0.f, -9.81f};
const Vector3 kWindForce = { 0.f, -1.f, 0.f };
const Vector3 kDamping = { .2f, .2f, .2f };

struct Particle {
    bool movable;
    Vector3 mass;
    Vector3 acceleration;
};

const int kWidth = 30;
const int kHeight = 20;

class Cloth {
public:
    Cloth(UserMesh& mesh) : mesh_(mesh), quadsCount_(static_cast<int>(mesh_.GetGeometry().TrianglesPositions.size() / (4 * 3))) {
        prev_ = mesh_.GetGeometry().UniquePositions; // in case of grid every position is unique
        assert(mesh_.GetGeometry().TrianglesPositions.size() % 4 == 0);
        InitConstraints();
        particles_.resize(prev_.size(), { true, Vector3(1.f), Vector3(0.f) });
        particles_[0].movable = false;
        particles_[1].movable = false;
        particles_[kWidth - 2].movable = false;
        particles_[kWidth - 1].movable = false;
    }

    void ApplyForce(const Vector3& force) {
        for (auto& p : particles_) {
            p.acceleration += force / p.mass;
        }
    }

    void ApplyWindForce(const Vector3& force) {
        const auto& triangles = mesh_.GetGeometry().TrianglesPositions;
        const auto& positions = mesh_.GetGeometry().UniquePositions;
        for (int i = 0; i < (int)triangles.size(); i += 3) {
            int p0 = triangles[i];
            int p1 = triangles[i+1];
            int p2 = triangles[i+2];
            const auto normal = Normalize(Cross(positions[p0] - positions[p1], positions[p1] - positions[p2]));
            const auto triForce = normal * Dot(normal, force);
            particles_[p0].acceleration += triForce / particles_[p0].mass;
            particles_[p0].acceleration += triForce / particles_[p1].mass;
            particles_[p0].acceleration += triForce / particles_[p2].mass;
        }
    }

    void Integrate(float dt) {
        auto& curr = mesh_.GetGeometryNonConst().UniquePositions;
        for (int i = 0; i < (int)particles_.size(); ++i) {
            if (particles_[i].movable) {
                const auto tmp = curr[i] + (curr[i] - prev_[i]) * kDamping + particles_[i].acceleration * dt * dt;
                prev_[i] = curr[i];
                curr[i] = tmp;
                particles_[i].acceleration = kZero; // We have just integrated it into velocity
            }
        }

        for (int i = 0; i < 1; ++i) {
            {
                const auto& s = constraints_[(size_t)ConstraintType::kShear];
                for (const auto& c : s) {
                    const Vector3 span = curr[c.v1] - curr[c.v2];
                    const Scalar ratioSq = LengthSq(span) / c.restingDistSq;
                    Scalar factor = 0.f;
                    if (ratioSq > kOne) {
                        factor = Scalar(kOne) - Scalar(kOne) * Scalar(2.f) / (Scalar(kOne) + ratioSq);
                    }
                    else if (ratioSq < kOne) {
                        factor = (Scalar(kOne) / ratioSq - Scalar(kOne)) / Scalar(100.f);
                    }
                    const Scalar ratio = SqrtEst(ratioSq);
                    const Scalar responsibility = particles_[c.v1].movable && particles_[c.v2].movable ? Scalar(2.f) : Scalar(1.f);
                    if (particles_[c.v1].movable) curr[c.v1] -= span / (responsibility * ratio) * factor;
                    if (particles_[c.v2].movable) curr[c.v2] += span / (responsibility * ratio) * factor;
                }
            }
        }
    }

private:
    void InitConstraints() {
        // Here we rely on the fact that triangles for every quad go in pairs next to each other
        // Based on this assumption we can create structural and shear constraints
        const auto& triangles = mesh_.GetGeometry().TrianglesPositions;
        unordered_map<pair<int, int>, Constraint, pairhash> structural, shear;
        for (int q = 0; q < quadsCount_; ++q) {
            unordered_map<pair<int, int>, int, pairhash> edgesCount;
            for (int t = 0; t < 4; ++t) {
                const int currentVertex = (q * 4 + t) * 3;
                ++edgesCount[minmax(triangles[currentVertex], triangles[currentVertex + 1])];
                ++edgesCount[minmax(triangles[currentVertex + 1], triangles[currentVertex + 2])];
                ++edgesCount[minmax(triangles[currentVertex + 2], triangles[currentVertex ])];
            }
            // Every edge repeated 4 times is stuctural and every edge repeated 2 times is shear contraint
            for (const auto& p : edgesCount) {
                const auto v1v2 = minmax(p.first.first, p.first.second);
                unordered_map<pair<int, int>, Constraint, pairhash> *container = nullptr;
                if (p.second == 4) {
                    container = &structural;
                }
                else if (p.second == 2) {
                    container = &shear;
                }
                else throw logic_error("Unexpected number of repetition for a constraint");
                if (container->find(v1v2) == container->end()) {
                    const float distSq = static_cast<float>(LengthSq(mesh_.GetGeometry().UniquePositions[v1v2.first] - mesh_.GetGeometry().UniquePositions[v1v2.second]));
                    (*container)[v1v2] = { v1v2.first, v1v2.second, distSq };
                }
            }
        }

        constraints_[(size_t)ConstraintType::kStructural].reserve(structural.size());
        for (const auto& c : structural) {
            constraints_[(size_t)ConstraintType::kStructural].push_back(c.second);
        }
        constraints_[(size_t)ConstraintType::kShear].reserve(shear.size());
        for (const auto& c : shear) {
            constraints_[(size_t)ConstraintType::kShear].push_back(c.second);
        }
    }

private:
    UserMesh &mesh_;
    vector<Common::Vector3> prev_;
    vector<Particle> particles_;
    const int quadsCount_;
    vector<Vector3> vertices_;
    array<vector<Constraint>, (size_t)ConstraintType::kConstraintTypesCount> constraints_;
};

class EulerianSolver {
    void SolveAll(std::vector<Constraint> constraints) {

    }
};

CONS_DEF(ClothSimulation);
ClothSimulation::~ClothSimulation() {}

void ClothSimulation::Init() {
    auto inputMesh = make_unique<InputMesh>("cloth");
    Helpers::CreateGridXy(kWidth, kHeight, 5.f, 5.f, Common::GeometryGenerator::QuadDivision::BothDiagonals, *inputMesh);
    inputScene_ = make_unique<InputScene>();
    inputScene_->AddMesh(move(inputMesh));

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    clothMesh_ = &scene_->GetMeshNonConst(scene_->SearchMesh("cloth"));
    InitBlankPhysicsData();
    cloth_ = make_unique<Cloth>(*clothMesh_);

    matDeformable_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kTurquesa(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *matDeformable_, *matDeformable_);

    clothRi_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void ClothSimulation::Step(float dt) {
    cloth_->ApplyForce(kGravityForce);
    cloth_->ApplyWindForce(kWindForce);
    cloth_->Integrate(dt);
    auto& mg = clothMesh_->GetGeometryNonConst();
    const auto& visualVertices = clothMesh_->GetInput().GetVisualVertices();
    for (int i = 0; i < (int)mg.UniqueVertices.size(); ++i) {
        mg.UniqueVertices[i].Position = mg.UniquePositions[visualVertices[i].PositionIndex].Store();
    }
    viewport_.UpdateRenderSubItemVertexData(*clothRi_, "cloth", (uint8*)mg.UniqueVertices.data());
}
