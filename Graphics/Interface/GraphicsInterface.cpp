#include "stdafx.h"
#include "GraphicsInterface.h"

#include <windows.h>

#include "Core\GraphicsCore.h"
#include "Core\RenderItem.h"
#include "Interface\InternalHandle.h"

using namespace Graphics;
using namespace std;

void grInit(HWND hWnd) {
    GraphicsCore::GetInstance().Initialize(hWnd);
}

void grShutdown() {}

void grBeginScene() {
    GraphicsCore::GetInstance().BeginScene();
}

void grEndScene() {
    GraphicsCore::GetInstance().EndScene();
}

void grDrawRenderItem(grRenderItem renderItem) {
    const RenderItem* ri = static_cast<RenderItemHandle*>(&renderItem)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItem(*ri);
}

grCommandContext grGetGraphicsContext() {
    auto cc = grCommandContext(GraphicsCore::GetInstance().GetCommandContext());
    return cc;
}

grRenderItem grCreateRenderItem(const void* vertices, const uint_t vertexSize, const uint_t verticesCount,
    const uint16* indices, const uint_t indicesCount, grCommandContext commandContext) {
    CommandContext* commandContextInternal = static_cast<CommandContextHandle*>(&commandContext)->GetValue();
    RenderItem *renderItem;
    RenderItem::Create(vertices, vertexSize, verticesCount, indices, sizeof(uint16), indicesCount, DXGI_FORMAT_R16_UINT, *commandContextInternal, renderItem);
    return grRenderItem(renderItem);
}

void grDestroyRenderItem(grRenderItem renderItem) {
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    delete ri;
}
