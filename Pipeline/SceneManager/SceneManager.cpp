#include "stdafx.h"

#include "SceneManager/SceneManager.h"

#include "3rdParty/YolaFbxImporter/YolaFbxImporter/YolaFbxImporter.h"
#include "Importer/FBXImporter.h"

#include "InputLevel/InputScene.h"
#include "InputLevel/FactoryFbx.h"
#include "InputLevel/InputMesh.h"
#include "InputLevel/InputCollider.h"
#include "Common/Print/DebugPrint.h"
#include "Common/Geometry/GeometryGenerator.h"
#include "Common/Math/Vector/all.h"
using namespace Common;

#include <fstream>
#include <sstream>

#include <DirectXMath.h>
using namespace DirectX;
using namespace std;
using namespace FBX;

namespace Pipeline
{
    namespace
    {
        XMMATRIX UpAxisRotation(::FBX::Scene::Axis upVector, int upVectorSign) {
            XMMATRIX rotation;
            // We have y-up, so adjust root matrix
            // In case you have mirrored objects you need to apply Utilities panel > Utilities rollout > Reset XForm
            // https://forums.autodesk.com/t5/3ds-max-forum/fbx-exports-resulting-in-flipped-objects/td-p/6044272
            switch (upVector)
            {
            case ::FBX::Scene::AXIS_X: assert("X can't be up axis"); break;
            case ::FBX::Scene::AXIS_Z:
                rotation = XMMatrixRotationX(-XM_PIDIV2);
                break;
            case ::FBX::Scene::AXIS_Y:
                rotation = XMMatrixIdentity();
                break;
            default:
                assert("Unknown Up Vector");
            }
            const float upVectorSignNormalized = upVectorSign > 0 ? 1.f : -1.f;
            rotation *= XMMatrixScaling(upVectorSignNormalized, upVectorSignNormalized, upVectorSignNormalized);
            return rotation;
        }

    }

    DEFINE_SINGLETON(SceneManager);
    constexpr const char fbx_ext[] = "fbx";
    constexpr const char hrm_ext[] = "hrm";

    SceneManager::~SceneManager() {}

    void SceneManager::Init() {
    }

    void SceneManager::Close() {
        scene_.reset();
    }

    void SceneManager::Load(const string& path, const string& filetitle) {
        const string relative = path + filetitle + "." + fbx_ext;
        if (!ifstream(relative.c_str()).good()) {
            vector<char> absolute(1024);
            GetFullPathName(relative.c_str(), (DWORD)absolute.size(), absolute.data(), nullptr);
            throw logic_error("File doesn't exist: " + string(absolute.data()));
        }

        const ::FBX::Scene *fbxScene = FbxImporter::GetInstance().GetScene(relative.c_str());
        const float scaleFactor = fbxScene->scaleFactor;

        colliders_ = move(LoadColliders((path + filetitle + "." + hrm_ext).c_str(), Matrix3(UpAxisRotation(fbxScene->upVector, fbxScene->upVectorSign))));

        scene_ = make_unique<InputScene>();

        ConvertScene(fbxScene, scaleFactor);
    }

    SceneManager::CollidersFbx SceneManager::LoadColliders(const std::string& filename, const Matrix3& upAxisRotation) {
        ifstream fs(filename, std::ios::in);
        if (fs.fail()) {
            char buf[256];
            strerror_s(buf, errno);
            Common::DebugPrintf("Error opening %s file: %s\n", filename.c_str(), buf);
        }

        const char* kName = "-name";
        const char* kType = "-type";
        const char* kWidth = "-width";
        const char* kLength = "-length";
        const char* kHeight = "-height";
        const char* kRadius = "-radius";
        const char* kTypeBox = "Box";
        const char* kTypeCapsule = "Capsule";
        const char* kTypeSphere = "Sphere";
        CollidersFbx colliders;
        if (fs.is_open()) {
            char name[100];
            char value[100];
            std::string line;
            while (getline(fs, line)) {
                istringstream iss(line);
                ColliderFbx curr;
                while (iss >> name >> value) {
                    if (!strcmp(name, kName)) curr.name = value;
                    else if (!strcmp(name, kWidth)) curr.width = stof(value);
                    else if (!strcmp(name, kLength)) curr.length = stof(value);
                    else if (!strcmp(name, kHeight)) curr.height = stof(value);
                    else if (!strcmp(name, kRadius)) curr.radius = stof(value);
                    else if (!strcmp(name, kType)) {
                        if (!strcmp(value, kTypeBox)) curr.type = ColliderType::kBox;
                        else if (!strcmp(value, kTypeSphere)) curr.type = ColliderType::kSphere;
                        else if (!strcmp(value, kTypeCapsule)) curr.type = ColliderType::kCapsule;
                        else throw "Can't parse hrm file";
                    }
                    else throw "Can't parse hrm file";
                }
                if (curr.type == ColliderType::kBox) {
                    Vector3 extents(curr.width, curr.height, curr.length);
                    extents = upAxisRotation * extents;
                    XMFLOAT3 e = extents.Store();
                    // e has the same coordinates as the box in 3ds max, namely if in 3ds max we have z-up, so height is in z-component
                    curr.width = abs(e.x); // Not to flip faces
                    curr.height = abs(e.y);
                    curr.length = abs(e.z);
                }
                colliders.push_back(curr);
            }
        }
        return colliders;
    }

    InputScene *SceneManager::GetScene() {
        return scene_.get();
    }

    void SceneManager::ConvertScene(const ::FBX::Scene *fbxScene, float scaleFactor) {
        FillSceneFromNode(fbxScene->rootNode, OrthogonalTransform(UpAxisRotation(fbxScene->upVector, fbxScene->upVectorSign)), scaleFactor);
    }

    bool IsOrthogonalMatrix(const Matrix3& m) {
        XMVECTOR scale, rot, trans;
        XMMatrixDecompose(&scale, &rot, &trans, m);
        return XMVector3NearEqual(g_XMOne, scale, XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f));
    }

    void SceneManager::FillSceneFromNode(const ::FBX::Node *node, const OrthogonalTransform &parent, float scaleFactor) {
        // It's not allowed to have scaled objects
        assert(IsOrthogonalMatrix(Matrix3(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node->transform.p)))) && "Scaling isn't allowed");

        OrthogonalTransform local(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node->transform.p)));
        local.SetTranslation(local.GetTranslation() * scaleFactor);

        for (uint32 i = 0; i < node->childrenCount; ++i) {
            FillSceneFromNode(node->childrenList[i], parent * local, scaleFactor);
        }

        if (node->element) {
            if (node->element->m_Type == ::FBX::Element::MESH) {
                const ColliderFbx* collider = GetColliderFbx(node->name);
                if (collider == nullptr) {
                    const auto xp = OrthoToAffine(parent).Store4x4();
                    const auto xl = OrthoToAffine(local).Store4x4();
                    Matrix4 mp(XMLoadFloat4x4(&xp));
                    Matrix4 ml(XMLoadFloat4x4(&xl));
                    Matrix4 mg = mp * ml;
                    const auto xg = OrthoToAffine(parent * local).Store4x4();
                    auto inputMesh = FactoryFbx::BuildMesh(node, parent * local, scaleFactor);
                    auto v = GeometryGenerator::ComputeVertices(inputMesh->GetTrianglesPositions(), inputMesh->GetTrianglesTexCoords());
                    inputMesh->SetVisualVertices(move(v.UniqueVertices));
                    inputMesh->SetTrianglesVertices(move(v.TrianglesVertices));
                    scene_->AddMesh(move(inputMesh));
                }
                else {
                    auto inputCollider = FactoryFbx::BuildCollider(node, parent, local, scaleFactor, *collider);
                    scene_->AddCollider(move(inputCollider));
                }
            }
        }
    }

    const ColliderFbx* SceneManager::GetColliderFbx(const char* name) const {
        for (auto& c : colliders_) {
            if (!strcmp(c.name.c_str(), name)) return &c;
        }
        return nullptr;
    }
}
