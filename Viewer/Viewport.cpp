#include "stdafx.h"
#include "Viewport.h"

#include "CameraController.h"
#include "GameInput.h"
#include "GeometryGenerator.h"
#include "Graphics\Interface\GraphicsInterface.h"

using namespace std;

namespace Viewer
{
    constexpr uint32 kSceneObjectsCountLimit = 100;
    constexpr uint32 kPassesCountLimit = 1;
    constexpr uint32 kMaterialsCountLimit = 2;
    constexpr uint32 kFrameResourcesCount = 3;

    const string kColliderMaterialKey = "collider";
    const string kRigidMaterialKey = "rigid";

    Viewport::Viewport(HWND hWnd) : hwnd_(hWnd) {
        grInit(hwnd_, { kSceneObjectsCountLimit, kPassesCountLimit, kMaterialsCountLimit, kFrameResourcesCount });

        RECT rect;
        GetClientRect(hWnd, &rect);
        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        grSetPerspective(height / width, DirectX::XM_PIDIV4, 1.f, 200.f);

        PrepareGeometry();

        materials_.insert(make_pair(kColliderMaterialKey, grCreateStandardMaterial(LibraryMaterial::kSilver, kColliderMaterialKey)));
        materials_.insert(make_pair(kRigidMaterialKey, grCreateStandardMaterial(LibraryMaterial::kTurquesa, kRigidMaterialKey)));

        grCreateDirectionalLight(XMFLOAT3(1.f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f));
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    uint_t Viewport::CreateRenderItem(const std::vector<RenderItemVerticesDesc>& viewportVerticesDescs, const std::vector<RenderItemTypeDesc>& viewportTypeDescs) {
        vector<grRenderSubItemDesc> descs;
        vector<grRenderVertices> vertices;
        vector<uint32> itemsToVertices;

        descs.reserve(viewportVerticesDescs.size() + viewportTypeDescs.size());
        itemsToVertices.reserve(viewportVerticesDescs.size() + viewportTypeDescs.size());
        vertices.reserve(viewportVerticesDescs.size());

        uint32 currentItem = 0;
        array<int, (size_t)PredefinedGeometryType::kSize> geometriesIndices;
        enum { kNoIndex = -1 };
        geometriesIndices.fill(kNoIndex);

        for (const auto& d : viewportTypeDescs) {
            uint_t geometryIndex = (uint_t)d.type;
            const grRenderSubItemDesc descEngine(d.name, d.transform, materials_.find(kColliderMaterialKey)->second);
            descs.push_back(descEngine);
            const auto& currentGeometry = geometries_[geometryIndex];
            if (geometriesIndices[geometryIndex] == kNoIndex) {
                geometriesIndices[geometryIndex] = (int)vertices.size();
                vertices.emplace_back((uint8*)(void*)currentGeometry.data(), (uint32)currentGeometry.size());
            }
            itemsToVertices.push_back(geometriesIndices[geometryIndex]);
            ++currentItem;
        }

        for (const auto& d : viewportVerticesDescs) {
            const grRenderSubItemDesc descEngine(d.name, d.transform, materials_.find(kRigidMaterialKey)->second);
            descs.push_back(descEngine);
            itemsToVertices.push_back((uint32)vertices.size());
            vertices.emplace_back((uint8*)(void*)d.vertices.data(), (uint32)d.vertices.size());
            ++currentItem;
        }

        grRenderItem ri = grCreateRenderItem(vertices, descs, itemsToVertices, sizeof(Vertex), grGetGraphicsContext());
        renderItems_.push_back(ri);
        return renderItems_.size() - 1;
    }

    void Viewport::BeforeDraw() {
        grBeginScene();
    }

    void Viewport::AfterDraw() {
        grEndScene();
    }

    void Viewport::DrawRenderItems() {
        for (const auto& ri : renderItems_) {
            grDrawRenderItem(ri);
        }
    }

    void Viewport::PrepareGeometry() {
        GeometryGenerator::CreateCube(geometries_[(size_t)PredefinedGeometryType::kBox]);
        GeometryGenerator::CreateSphere(geometries_[(size_t)PredefinedGeometryType::kSphere], 2);
    }
}