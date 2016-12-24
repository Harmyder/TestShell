#include "stdafx.h"
#include "Viewport.h"

#include "CameraController.h"
#include "GameInput.h"

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
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    uint_t Viewport::CreateRenderIndexedItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, const XMFLOAT4X4& transform) {
        grRenderIndexedItem ri = grCreateRenderIndexedItem(vertices.data(), sizeof(Vertex), vertices.size(), indices.data(), indices.size(), transform, grGetGraphicsContext());
        renderIndexedItems_.push_back(ri);
        return renderIndexedItems_.size() - 1;
    }

    uint_t Viewport::CreateRenderItem(const std::vector<Vertex>& vertices, const XMFLOAT4X4& transform) {
        grRenderItem ri = grCreateRenderItem(vertices.data(), sizeof(Vertex), vertices.size(), transform, grGetGraphicsContext());
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
        for (const auto& ri : renderIndexedItems_) {
            grDrawRenderIndexedItem(ri);
        }
    }

}