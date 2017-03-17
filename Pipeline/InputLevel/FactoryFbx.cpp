// http://www.walkerb.net/blog/dx-4/

#include "stdafx.h"

#include "InputLevel/FactoryFbx.h"
#include "InputLevel/InputMesh.h"
#include "InputLevel/InputCollider.h"
#include "3rdParty/YolaFbxImporter/YolaFbxImporter/YolaFbxImporter.h"

using namespace DirectX;
using namespace std;

namespace Pipeline
{
    unique_ptr<InputMesh> FactoryFbx::BuildMesh(const ::FBX::Node *fbxNode, const XMFLOAT4X4 &globalTransform, float scaleFactor)
    {
        assert(fbxNode->element->m_Type == ::FBX::Element::MESH);

        const ::FBX::Mesh *fbxMesh = static_cast<const ::FBX::Mesh *>(fbxNode->element);

        auto mesh = make_unique<InputMesh>(fbxNode->name);
        mesh->SetTransform(globalTransform);

        // Import positions
        {
            const uint32 verticesCount = fbxMesh->verticesCount;
            std::vector<XMFLOAT3> vertices(verticesCount);
            for (uint32 v = 0; v < verticesCount; v++)
            {
                vertices[v].x = fbxMesh->vertices[v].p[0] * scaleFactor;
                vertices[v].y = fbxMesh->vertices[v].p[1] * scaleFactor;
                vertices[v].z = fbxMesh->vertices[v].p[2] * scaleFactor;
            }
            mesh->SetPositions(vertices);
        }

        // Import normals
        {
            // We support only one normal per vertex, thus we don't support smooth groups
            const uint32 normalsCount = fbxMesh->verticesCount;
            std::vector<XMFLOAT3> normals(normalsCount);
            ZeroMemory(&normals[0], normals.size() * sizeof(normals[0]));
            const uint32 trianglesCount = fbxMesh->trianglesCount;
            for (uint32 t = 0; t < trianglesCount; t++)
            {
                for (uint32 i = 0; i < 3; i++)
                {
                    uint16 trianglesPositionIndex = static_cast<uint16>(fbxMesh->triangles[t].v[i]);
                    uint16 trianglesNormalIndex = static_cast<uint16>(fbxMesh->triangles[t].n[i]);
                    normals[trianglesPositionIndex].x += fbxMesh->normals[trianglesNormalIndex].p[0];
                    normals[trianglesPositionIndex].y += fbxMesh->normals[trianglesNormalIndex].p[1];
                    normals[trianglesPositionIndex].z += fbxMesh->normals[trianglesNormalIndex].p[2];
                }
            }
            for (uint32 n = 0; n < normalsCount; n++)
            {
                XMVECTOR normalXM = XMLoadFloat3(&normals[n]);
                const XMVECTOR length = XMVector3Length(normalXM);
                assert(XMVector4Greater(length, XMVectorSet(1e-6f, 1e-6f, 1e-6f, 1e-6f)) && "Too small normal");
                normalXM = normalXM / length;
                XMStoreFloat3(&normals[n], normalXM);
            }
            mesh->SetNormals(normals);
        }

        // Import triangles positions
        {
            const uint32 trianglesCount = fbxMesh->trianglesCount;
            std::vector<uint16> trianglesPositions(trianglesCount * 3);
            for (uint32 t = 0; t < trianglesCount; t++)
            {
                trianglesPositions[t * 3 + 0] = static_cast<uint16>(fbxMesh->triangles[t].v[0]);
                trianglesPositions[t * 3 + 1] = static_cast<uint16>(fbxMesh->triangles[t].v[1]);
                trianglesPositions[t * 3 + 2] = static_cast<uint16>(fbxMesh->triangles[t].v[2]);
            }
            mesh->SetTrianglesPositions(trianglesPositions);
        }

        // Import texCoords
        {
            const uint32 uvsCount = fbxMesh->uvsCount;
            std::vector<XMFLOAT2> uvs(uvsCount);
            for (uint32 uvIndex = 0; uvIndex < uvsCount; uvIndex++)
            {
                uvs[uvIndex].x = fbxMesh->uvs[uvIndex].p[0];
                uvs[uvIndex].y = fbxMesh->uvs[uvIndex].p[1];
            }
            mesh->SetTexCoords(uvs);
        }

        // Import triangles texCoords
        {
            const uint32 trianglesCount = fbxMesh->trianglesCount;
            std::vector<uint16> trianglesTexCoords(trianglesCount * 3);
            for (uint32 t = 0; t < trianglesCount; t++)
            {
                trianglesTexCoords[t * 3 + 0] = static_cast<uint16>(fbxMesh->triangles[t].uv[0]);
                trianglesTexCoords[t * 3 + 1] = static_cast<uint16>(fbxMesh->triangles[t].uv[1]);
                trianglesTexCoords[t * 3 + 2] = static_cast<uint16>(fbxMesh->triangles[t].uv[2]);
            }
            mesh->SetTrianglesTexCoords(trianglesTexCoords);
        }

        return mesh;
    }

    std::unique_ptr<InputCollider> FactoryFbx::BuildCollider(const FBX::Node *fbxNode, const XMFLOAT4X4 &globalTransform, float scaleFactor, const ColliderFbx& colliderFbx) {
        assert(fbxNode->element->m_Type == ::FBX::Element::MESH);

        unique_ptr<InputCollider> collider;
        switch (colliderFbx.type) {
        case ColliderType::kBox:
            collider = make_unique<InputBoxCollider>(
                fbxNode->name,
                globalTransform,
                colliderFbx.x * scaleFactor,
                colliderFbx.y * scaleFactor,
                colliderFbx.z * scaleFactor
                );
            break;
        case ColliderType::kSphere:
            collider = make_unique<InputSphereCollider>(
                fbxNode->name,
                globalTransform,
                colliderFbx.radius * scaleFactor
                );
            break;
        case ColliderType::kCapsule:
            collider = make_unique<InputCapsuleCollider>(
                fbxNode->name,
                globalTransform,
                colliderFbx.height * scaleFactor,
                colliderFbx.radius * scaleFactor
                );
            break;
        }

        return collider;
    }
}
