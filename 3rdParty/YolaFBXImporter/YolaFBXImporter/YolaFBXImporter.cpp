// YolaFBXImporter.cpp : Defines the exported functions for the DLL application.
// http://www.walkerb.net/blog/dx-4/
// https://github.com/fcoulombe/kinevox/blob/master/program/tools/common/FBXManager.cpp

#include "stdafx.h"

#include <fbxsdk.h>

#include <stdio.h>

#define DLLEXPORT
#include "YolaFBXImporter.h"

#include "DebugPrint.h"

namespace FBX 
{

namespace 
{
    template <class T>
    void LoadLayer(Mesh *mesh, const FbxLayerElementTemplate<T> *eLayerElement, unsigned int (Triangle::*_pm)[3])
    {
        // We require that all polygons be triangles
        const int trianglesCount = mesh->trianglesCount;

        switch(eLayerElement->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            for (int p = 0; p < trianglesCount; p++)
            {
                Triangle &tri = mesh->triangles[p];
                switch (eLayerElement->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        (tri.*_pm)[0] = tri.v[0];
                        (tri.*_pm)[1] = tri.v[1];
                        (tri.*_pm)[2] = tri.v[2];
                        break;
                    }
                case FbxGeometryElement::eIndexToDirect:
                    {
                        for (int tv = 0; tv < 3; tv++)
                        {
                            (tri.*_pm)[0] = eLayerElement->GetIndexArray().GetAt(tri.v[0]);
                            (tri.*_pm)[1] = eLayerElement->GetIndexArray().GetAt(tri.v[1]);
                            (tri.*_pm)[2] = eLayerElement->GetIndexArray().GetAt(tri.v[2]);
                        }
                        break;
                    }
                default:
                    _ASSERT(0 && "Unknown reference mode.");
                }                
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            for (int p = 0; p < trianglesCount; p++)
            {
                const int basePolygonVertexIndex = p * 3;
                Triangle &tri = mesh->triangles[p];
                switch (eLayerElement->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        (tri.*_pm)[0] = basePolygonVertexIndex;
                        (tri.*_pm)[1] = basePolygonVertexIndex + 1;
                        (tri.*_pm)[2] = basePolygonVertexIndex + 2;
                        break;
                    }
                case FbxGeometryElement::eIndexToDirect:
                    {
                        for (int tv = 0; tv < 3; tv++)
                        {
                            (tri.*_pm)[0] = eLayerElement->GetIndexArray().GetAt(basePolygonVertexIndex);
                            (tri.*_pm)[1] = eLayerElement->GetIndexArray().GetAt(basePolygonVertexIndex + 1);
                            (tri.*_pm)[2] = eLayerElement->GetIndexArray().GetAt(basePolygonVertexIndex + 2);
                        }
                        break;
                    }
                default:
                    _ASSERT(0 && "Unknown reference mode.");
                }                
            }
            break;
        default:
            _ASSERT(0 && "Unknown mapping mode.");
        }
    }
}

template<class _Element> _Element *CreateElement()
{
    _Element *element = new _Element;
    ZeroMemory(element, sizeof(element));
    element->m_Type = _Element::TYPE;

    return element;
}

void FreeElement(FBX::Mesh *mesh)
{
    delete[] mesh->triangles;
    delete[] mesh->uvs;
    delete[] mesh->normals;
    delete[] mesh->vertices;
    delete mesh;
}

Node *CreateNode()
{
    Node *result = new Node;

    result->parent     = NULL;
    result->childrenCount = 0;
    result->childrenList  = NULL;
    result->element    = NULL;

    return result;
}

void FreeNode(Node *node)
{
    for (unsigned int i = 0; i < node->childrenCount; i++)
    {
        FreeNode(node->childrenList[i]);
    }
    if (node->element)
    {
        switch (node->element->m_Type)
        {
        case Element::MESH:
            FreeElement(static_cast<Mesh*>(node->element));
            break;
        default:
            _ASSERT(0 && "Unknown element type.");
        }
    }
    delete node->name;
    delete[] node->childrenList;
    delete node;
}

Node *ImportNode(FbxNode* fbxNode)
{

    Node *node = NULL;
    if(fbxNode)
    {
        node = CreateNode();

        const int childrenCount = fbxNode->GetChildCount();
        node->childrenCount = childrenCount;
        node->childrenList = new Node*[childrenCount];
        for(int childIndex = 0; childIndex < childrenCount; childIndex++)
        {
            node->childrenList[childIndex] = ImportNode(fbxNode->GetChild(childIndex));
        }

        FbxAMatrix defaultGlobalTransform;
        defaultGlobalTransform.SetR(fbxNode->LclRotation.Get());
        defaultGlobalTransform.SetS(fbxNode->LclScaling.Get());
        defaultGlobalTransform.SetT(fbxNode->LclTranslation.Get());
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                node->transform.p[i][j] = static_cast<float>(defaultGlobalTransform.Double44()[i][j]);                
            }
        }

        const char *name = fbxNode->GetName();
        const int length = static_cast<int>(strlen(name) + 1);
        node->name = new char[length];
        strcpy_s(node->name, length, name);
    
        if (fbxNode->GetNodeAttribute() != NULL)
        {
            FbxNodeAttribute::EType attributeType = fbxNode->GetNodeAttribute()->GetAttributeType();
    
            if(attributeType == FbxNodeAttribute::eMesh)
            {
                FbxMesh* pMesh = (FbxMesh*)fbxNode->GetNodeAttribute();

                const auto vertexColorCount = pMesh->GetElementVertexColorCount();
                if (vertexColorCount > 0) {
                    FbxLayerElementVertexColor* vertexColor = pMesh->GetElementVertexColor();
                    // For now i can't check layer name, looks like it doesn't get exported from 3ds max
                    // if (!strcmp(vertexColor->GetName(), "Collider")) {}
                }

                Mesh *mesh = CreateElement<Mesh>();
                node->element = mesh;
    
                // Vertices positions
                // FBX SDK Refernce Guide: A control point is a XYZ coordinate, it is synonym of vertex.
                const int verticesCount = pMesh->GetControlPointsCount();
                mesh->verticesCount = verticesCount;
                mesh->vertices = new Vector4[verticesCount];
                FbxVector4* pVertices = pMesh->GetControlPoints();
                for (int vertexIndex = 0; vertexIndex < verticesCount; vertexIndex++)
                {
                    mesh->vertices[vertexIndex].p[0] = static_cast<float>(pVertices[vertexIndex][0]);
                    mesh->vertices[vertexIndex].p[1] = static_cast<float>(pVertices[vertexIndex][1]);
                    mesh->vertices[vertexIndex].p[2] = static_cast<float>(pVertices[vertexIndex][2]);
                    mesh->vertices[vertexIndex].p[3] = static_cast<float>(pVertices[vertexIndex][3]);
                }
            
                // Triangle indices
                // FBX SDK Reference Guide: A polygon vertex is an index to a control point, it is synonym of vertex index.
                const int polygonsCount = pMesh->GetPolygonCount();
                mesh->trianglesCount = polygonsCount;
                mesh->triangles = new Triangle[polygonsCount];
                for (int polygonIndex = 0; polygonIndex < polygonsCount; polygonIndex++)
                {
                    const int triangleVerticesCount = 3;
                    // To get mesh triangulated you can convert it to "Editable Poly"
                    // then select all vertices (Crtl+A) and select "Connect" in the context menu
                    _ASSERT(pMesh->GetPolygonSize(polygonIndex) == triangleVerticesCount);
                
                    Triangle &triangle = mesh->triangles[polygonIndex];
                    for (int k = 0; k < triangleVerticesCount; k++)
                    {
                        triangle.v[k] = pMesh->GetPolygonVertex(polygonIndex, k);
                    }
                }
    
                // Normals
                FbxLayer *normalLayer = pMesh->GetLayer(0, FbxLayerElement::eNormal);
                const FbxLayerElementNormal* eNormal = normalLayer->GetNormals();
                const int normalsCount = eNormal->GetDirectArray().GetCount();
                // a. Import normals values
                mesh->normalsCount = normalsCount;
                mesh->normals = new Vector4[normalsCount];
                for (int n = 0; n < normalsCount; n++)
                {
                    const FbxVector4 &normal = eNormal->GetDirectArray().GetAt(n);
                    mesh->normals[n].p[0] = static_cast<float>(normal[0]);
                    mesh->normals[n].p[1] = static_cast<float>(normal[1]);
                    mesh->normals[n].p[2] = static_cast<float>(normal[2]);
                    mesh->normals[n].p[3] = static_cast<float>(normal[3]);
                }
                // b. Import their triangles indices
                LoadLayer(mesh, eNormal, &Triangle::n);

                // Texture coordinates
                FbxLayer *uvLayer = pMesh->GetLayer(0, FbxLayerElement::eUV);
                if (uvLayer)
                {
                    const FbxLayerElementUV* eUV = uvLayer->GetUVs();
                    const int uvsCount = eUV->GetDirectArray().GetCount();
                    // a. Import uvs values
                    mesh->uvsCount = uvsCount;
                    mesh->uvs = new Vector2[uvsCount];
                    for (int n = 0; n < uvsCount; n++)
                    {
                        const FbxVector2 &uv = eUV->GetDirectArray().GetAt(n);
                        mesh->uvs[n].p[0] = static_cast<float>(uv[0]);
                        mesh->uvs[n].p[1] = static_cast<float>(uv[1]);
                    }
                    // b. Import their triangles indices
                    LoadLayer(mesh, eUV, &Triangle::uv);
                }
            }
        }
    }

    return node;
}

Scene *ImportScene(const char *filePath)
{
    FbxManager *sdkManager = FbxManager::Create();
    PrintErrorOnCreation(sdkManager, "FbxManager");
    if(!sdkManager) return NULL;

    FbxImporter* importer = FbxImporter::Create(sdkManager,"");
    PrintErrorOnCreation(importer, "FbxImporter");
    if(!importer) return NULL;

    FbxScene* fbxScene = FbxScene::Create(sdkManager,"");
    PrintErrorOnCreation(fbxScene, "FbxScene");
    if(!fbxScene) return NULL;

    bool bSuccess = importer->Initialize(filePath, -1, sdkManager->GetIOSettings());
    PrintErroOnFunctionCall(bSuccess, false, "importer->Initialize"); 
    if(!bSuccess) return NULL;

    bSuccess = importer->Import(fbxScene);
    PrintErroOnFunctionCall(bSuccess, false, "importer->Import"); 
    if(!bSuccess) return NULL;

    importer->Destroy();

    Scene *scene = new Scene();
    scene->rootNode = ImportNode(fbxScene->GetRootNode());

    switch (fbxScene->GetGlobalSettings().GetAxisSystem().GetUpVector(scene->upVectorSign))
    {
    case FbxAxisSystem::eXAxis:
        scene->upVector = Scene::AXIS_X;
        break;
    case FbxAxisSystem::eYAxis:
        scene->upVector = Scene::AXIS_Y;
        break;
    case FbxAxisSystem::eZAxis:
        scene->upVector = Scene::AXIS_Z;
        break;
    }

    scene->scaleFactor = static_cast<float>(fbxScene->GetGlobalSettings().GetSystemUnit().GetScaleFactor()) * 0.01f; // It is in centimeters, so multiply by 0.01

    sdkManager->Destroy();

    return scene;
}

void FreeScene(Scene *scene)
{
    FreeNode(scene->rootNode);
    delete scene;
}

}

_declspec(dllexport) const FBX::Scene *LoadFbxFile(const char *fbxFilename)
{
    FBX::Scene *scene = FBX::ImportScene(fbxFilename);
    return scene;
}

_declspec(dllexport) void FreeScene(const FBX::Scene *scene)
{
    FBX::FreeScene(const_cast<FBX::Scene *>(scene));
}
