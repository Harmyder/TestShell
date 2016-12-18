#include "stdafx.h"
#include "Viewport.h"

#include "Camera.h"
#include "CameraController.h"
#include "GameInput.h"
#include "RenderItem.h"

#include "Graphics\GraphicsCore.h"
#include "Graphics\GpuBuffer.h"

using namespace std;
using namespace Graphics;

namespace Viewer
{
    Viewport::Viewport(HWND hWnd, const GameInput& gameInput) :
        hwnd_(hWnd),
        camera_(make_unique<Camera>()),
        cameraCtrl_(make_unique<CameraController>(*camera_, gameInput))
    {
        graphicsCore_->Initialize(hwnd_);
    }

    Viewport::~Viewport() {}

    void Viewport::CreateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, RenderItem &renderItem) {
        auto commandContext = graphicsCore_->GetCommandContext();
        renderItem.vertexBuffer_->Create("renderItem", vertices.size(), sizeof(Vertex), vertices.data(), commandContext);
        renderItem.indexBuffer_->Create("renderItem", indices.size(), sizeof(Vertex), indices.data(), commandContext);


    }

    void Viewport::UpdateRenderItem(const std::vector<Vertex>& vertices, const std::vector<uint16>& indices, RenderItem &renderItem) {

    }
}