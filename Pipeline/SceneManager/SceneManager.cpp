#include "stdafx.h"

#include "SceneManager/SceneManager.h"

#include "3rdParty/YolaFbxImporter/YolaFbxImporter/YolaFbxImporter.h"
#include "Importer/FBXImporter.h"

#include "InputLevel/Scene/InputScene.h"
#include "InputLevel/Factory/FactoryFbx.h"
#include "InputLevel/Mesh/InputMesh.h"

#include <DirectXMath.h>
using namespace DirectX;
using namespace std;
using namespace FBX;

namespace Pipeline
{
    DEFINE_SINGLETON(SceneManager);

    // **************************************************************************************
    SceneManager::~SceneManager() {}

    // **************************************************************************************
    void SceneManager::Init()
    {
    }

    // **************************************************************************************
    void SceneManager::Close()
    {
        scene_.reset();
    }

    // **************************************************************************************
    void SceneManager::Load(const char * filename)
    {
        const ::FBX::Scene *fbxScene = FbxImporter::GetInstance().GetScene(filename);
        const float scaleFactor = fbxScene->scaleFactor;

        scene_ = make_unique<InputScene>();

        ConvertScene(fbxScene, scaleFactor);
    }

    // **************************************************************************************
    InputScene *SceneManager::GetScene()
    {
        return scene_.get();
    }

    // **************************************************************************************
    void SceneManager::ConvertScene(const ::FBX::Scene *fbxScene, float scaleFactor)
    {
        XMMATRIX rotation;
        // We have y-up, so adjust root matrix
        switch (fbxScene->upVector)
        {
        case ::FBX::Scene::AXIS_X:
            rotation = XMMatrixRotationZ(-XM_PIDIV2);
            break;
        case ::FBX::Scene::AXIS_Y:
            rotation = XMMatrixIdentity();
            break;
        case ::FBX::Scene::AXIS_Z:
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

    // **************************************************************************************
    void SceneManager::FillSceneFromNode(const ::FBX::Node *node, const XMFLOAT4X4 &transform, float scaleFactor)
    {
        XMMATRIX currentTransformXM = XMLoadFloat4x4(&transform);
        const XMMATRIX localTransformXM = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node->transform.p));
        currentTransformXM = localTransformXM * currentTransformXM;

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
                FactoryFbx factory;
                InputMesh *inputMesh = new InputMesh;
                factory.Build(*inputMesh, node, currentTransform, scaleFactor);
                inputMesh->ComputeVertices();

                scene_->AddMesh(inputMesh);
            }
        }
    }
}
