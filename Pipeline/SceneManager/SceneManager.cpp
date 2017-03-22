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
using namespace Common;

#include <fstream>
#include <sstream>

#include <DirectXMath.h>
using namespace DirectX;
using namespace std;
using namespace FBX;

namespace Pipeline
{
    DEFINE_SINGLETON(SceneManager);
    constexpr const char fbx_ext[] = "fbx";
    constexpr const char hrm_ext[] = "hrm";

    SceneManager::~SceneManager() {}

    void SceneManager::Init()
    {
    }

    void SceneManager::Close()
    {
        scene_.reset();
    }

    void SceneManager::Load(const string& path, const string& filetitle)
    {
        const ::FBX::Scene *fbxScene = FbxImporter::GetInstance().GetScene((path + filetitle + "." + fbx_ext).c_str());
        const float scaleFactor = fbxScene->scaleFactor;

        colliders_ = move(LoadColliders((path + filetitle + "." + hrm_ext).c_str()));

        scene_ = make_unique<InputScene>();

        ConvertScene(fbxScene, scaleFactor);
    }

    SceneManager::CollidersFbx SceneManager::LoadColliders(const std::string& filename) {
        ifstream fs(filename, std::ios::in);
        if (fs.fail()) {
            char buf[256];
            strerror_s(buf, errno);
            Common::DebugPrintf("Error opening %s file: %s\n", filename.c_str(), buf);
        }

        const char* kName = "-name";
        const char* kType = "-type";
        const char* kX = "-x";
        const char* kY = "-y";
        const char* kZ = "-z";
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
                    else if (!strcmp(name, kX)) curr.x = stof(value);
                    else if (!strcmp(name, kY)) curr.y = stof(value);
                    else if (!strcmp(name, kZ)) curr.z = stof(value);
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
                colliders.push_back(curr);
            }
        }
        return colliders;
    }

    InputScene *SceneManager::GetScene()
    {
        return scene_.get();
    }

    void SceneManager::ConvertScene(const ::FBX::Scene *fbxScene, float scaleFactor)
    {
        XMMATRIX rotation;
        // We have y-up, so adjust root matrix
        // In case you have mirrored objects you need to apply Utilities panel > Utilities rollout > Reset XForm
        // https://forums.autodesk.com/t5/3ds-max-forum/fbx-exports-resulting-in-flipped-objects/td-p/6044272
        switch (fbxScene->upVector)
        {
        case ::FBX::Scene::AXIS_X: assert("X can't be up axis"); break;
        case ::FBX::Scene::AXIS_Z: assert("Z can't be up axis"); break;
        case ::FBX::Scene::AXIS_Y:
            rotation = XMMatrixRotationX(-XM_PIDIV2);
            break;
        default:
            assert("Unknown Up Vector");
        }
        const float upVectorSign = fbxScene->upVectorSign > 0 ? 1.f : -1.f;
        rotation *= XMMatrixScaling(upVectorSign, upVectorSign, upVectorSign);
        XMFLOAT4X4 r;
        XMStoreFloat4x4(&r, rotation);
        FillSceneFromNode(fbxScene->rootNode, r, scaleFactor);
    }

    void SceneManager::FillSceneFromNode(const ::FBX::Node *node, const XMFLOAT4X4 &transform, float scaleFactor)
    {
        XMMATRIX currentTransformXM = XMLoadFloat4x4(&transform);
        const XMMATRIX localTransformXM = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node->transform.p));
        currentTransformXM = currentTransformXM * localTransformXM;

        for (uint32 i = 0; i < node->childrenCount; ++i)
        {
            XMFLOAT4X4 t;
            XMStoreFloat4x4(&t, currentTransformXM);
            FillSceneFromNode(node->childrenList[i], t, scaleFactor);
        }

        XMFLOAT4X4 currentTransform;
        XMStoreFloat4x4(&currentTransform, currentTransformXM);
        currentTransform._41 *= scaleFactor;
        currentTransform._42 *= scaleFactor;
        currentTransform._43 *= scaleFactor;
        if (node->element)
        {
            if (node->element->m_Type == ::FBX::Element::MESH)
            {
                const ColliderFbx* collider = GetColliderFbx(node->name);
                if (collider == nullptr) {
                    auto inputMesh = FactoryFbx::BuildMesh(node, currentTransform, scaleFactor);
                    auto v = GeometryGenerator::ComputeVertices(inputMesh->GetTrianglesPositions(), inputMesh->GetTrianglesTexCoords());
                    inputMesh->SetVisualVertices(move(v.UniqueVertices));
                    inputMesh->SetTrianglesVertices(move(v.TrianglesVertices));
                    scene_->AddMesh(move(inputMesh));
                }
                else {
                    auto inputCollider = FactoryFbx::BuildCollider(node, currentTransform, scaleFactor, *collider);
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
