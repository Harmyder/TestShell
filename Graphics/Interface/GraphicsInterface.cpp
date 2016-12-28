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

void grSetPerspective(float heightOverWidth, float fov, float nearClipPlane, float farClipPlane) {
    auto& camera = GraphicsCore::GetInstance().GetCamera();
    camera.SetAspectRatio(heightOverWidth);
    camera.SetVerticalFOV(fov);
    camera.SetNearFarClipPlanes(nearClipPlane, farClipPlane);
}

void grBeginScene() {
    GraphicsCore::GetInstance().BeginScene();
}

void grEndScene() {
    GraphicsCore::GetInstance().EndScene();
}

void grDrawRenderItem(grRenderItem renderItem) {
    RenderItem* ri = static_cast<RenderItemHandle*>(&renderItem)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItem(*ri);
}

grCommandContext grGetGraphicsContext() {
    auto cc = grCommandContext(GraphicsCore::GetInstance().GetCommandContext());
    return cc;
}

grRenderItem grCreateRenderItem(
    const std::vector<grRenderVertices>& renderVertices,
    const std::vector<grRenderItemDesc>& renderItems,
    const std::vector<uint32>& itemsToVertices,
    uint32 vertexSize,
    grCommandContext commandContext)
{
    CommandContext* commandContextInternal = static_cast<CommandContextHandle*>(&commandContext)->GetValue();
    auto renderItemsInternal = (vector<RenderItemDesc>*)((void*)&renderItems);
    auto renderVerticesInternal = (vector<RenderVerticesDesc>*)((void*)&renderVertices);
    RenderItem *renderItem;
    RenderItem::Create(*renderItemsInternal, *renderVerticesInternal, itemsToVertices, vertexSize, *commandContextInternal, renderItem);
    return grRenderItem(renderItem);
}

void grDestroyRenderItem(grRenderItem renderItem) {
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    delete ri;
}

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine) {
    GraphicsCore::GetInstance().GetCamera().SetAffineTransform(affine);
}
