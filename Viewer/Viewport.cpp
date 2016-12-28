#include "stdafx.h"
#include "Viewport.h"

#include "CameraController.h"
#include "GameInput.h"
#include "GeometryGenerator.h"
#include "Graphics\Interface\GraphicsInterface.h"

using namespace std;
using namespace Graphics;

namespace Viewer
{
    Viewport::Viewport(HWND hWnd) : hwnd_(hWnd) {
        grInit(hwnd_);

        RECT rect;
        GetClientRect(hWnd, &rect);
        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        grSetPerspective(height / width, DirectX::XM_PIDIV4, 1.f, 200.f);

        PrepareGeometry();
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    uint_t Viewport::CreateRenderItem(const std::vector<RenderItemVerticesDesc>& viewportVerticesDescs, const std::vector<RenderItemTypeDesc>& viewportTypeDescs) {
        vector<grRenderItemDesc> descs;
        vector<grRenderVertices> vertices;
        vector<uint32> itemsToVertices;

        descs.reserve(viewportVerticesDescs.size() + viewportTypeDescs.size());
        itemsToVertices.reserve(viewportVerticesDescs.size() + viewportTypeDescs.size());
        vertices.reserve(viewportVerticesDescs.size());

        uint32 currentItem = 0;
        array<int, (size_t)PredefinedGeometryType::kSize> geometriesIndices;
        constexpr int NOINDEX = -1;
        geometriesIndices.fill(NOINDEX);

        for (const auto& d : viewportTypeDescs) {
            uint_t geometryIndex = (uint_t)d.type;
            const grRenderItemDesc descEngine(d.name, d.transform);
            descs.push_back(descEngine);
            const auto& currentGeometry = geometries_[geometryIndex];
            if (geometriesIndices[geometryIndex] == NOINDEX) {
                geometriesIndices[geometryIndex] = (int)vertices.size();
            }
            itemsToVertices.push_back(geometriesIndices[geometryIndex]);
            vertices.emplace_back((uint8*)(void*)currentGeometry.data(), (uint32)currentGeometry.size());
            ++currentItem;
        }

        for (const auto& d : viewportVerticesDescs) {
            const grRenderItemDesc descEngine(d.name, d.transform);
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