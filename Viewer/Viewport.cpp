#include "stdafx.h"
#include "Viewport.h"

#include "Camera.h"
#include "CameraController.h"
#include "GameInput.h"

#include "Graphics\Interface\GraphicsInterface.h"

using namespace std;
using namespace Graphics;

namespace Viewer
{
    Viewport::Viewport(HWND hWnd, const GameInput& gameInput) :
        hwnd_(hWnd),
        camera_(make_unique<Camera>()),
        cameraCtrl_(make_unique<CameraController>(*camera_, gameInput))
    {
        grInit(hwnd_);
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    uint_t Viewport::CreateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices) {
        grRenderItem ri = grCreateRenderItem(vertices.data(), sizeof(Vertex), vertices.size(), indices.data(), indices.size(), grGetGraphicsContext());
        renderItems_.push_back(ri);
        return renderItems_.size() - 1;
    }

    void Viewport::UpdateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, uint_t index) {
        vertices, indices, index;
        throw;
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