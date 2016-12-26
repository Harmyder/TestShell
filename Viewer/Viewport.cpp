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

    uint_t Viewport::CreateRenderItem(const vector<RenderItemDesc>& viewportDescs) {
        vector<grRenderItemDesc> descs;
        descs.reserve(viewportDescs.size());
        for (const auto& d : viewportDescs) {
            const grRenderItemDesc descEngine(d.name, (uint8*)(void*)d.vertices.data(), (uint32)d.vertices.size(), d.transform);
            descs.push_back(descEngine);
        }
        grRenderItem ri = grCreateRenderItems(descs, sizeof(Vertex), grGetGraphicsContext());
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

}