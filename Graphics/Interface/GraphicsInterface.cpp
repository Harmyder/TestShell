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
    const RenderItem* ri = static_cast<RenderItemHandle*>(&renderItem)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItem(*ri);
}

void grDrawRenderIndexedItem(grRenderIndexedItem renderIndexedItem) {
    const RenderIndexedItem* ri = static_cast<RenderIndexedItemHandle*>(&renderIndexedItem)->GetValue();
    GraphicsCore::GetInstance().DrawRenderIndexedItem(*ri);
}

grCommandContext grGetGraphicsContext() {
    auto cc = grCommandContext(GraphicsCore::GetInstance().GetCommandContext());
    return cc;
}

grRenderIndexedItem grCreateRenderIndexedItem(
    const void* vertices, 
    const uint_t vertexSize, 
    const uint_t verticesCount,
    const uint16* indices, 
    const uint_t indicesCount,
    const XMFLOAT4X4& transform,
    grCommandContext commandContext) 
{
    CommandContext* commandContextInternal = static_cast<CommandContextHandle*>(&commandContext)->GetValue();
    RenderIndexedItem *renderIndexedItem;
    RenderIndexedItem::Create(vertices, vertexSize, verticesCount, indices, sizeof(uint16), indicesCount, DXGI_FORMAT_R16_UINT, transform, *commandContextInternal, renderIndexedItem);
    return grRenderIndexedItem(renderIndexedItem);
}

grRenderItem grCreateRenderItem(
    const void* vertices,
    const uint_t vertexSize,
    const uint_t verticesCount,
    const XMFLOAT4X4& transform,
    grCommandContext commandContext)
{
    CommandContext* commandContextInternal = static_cast<CommandContextHandle*>(&commandContext)->GetValue();
    RenderItem *renderItem;
    RenderItem::Create(vertices, vertexSize, verticesCount, transform, *commandContextInternal, renderItem);
    return grRenderItem(renderItem);
}

void grDestroyRenderItem(grRenderItem renderItem) {
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    delete ri;
}

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine) {
    GraphicsCore::GetInstance().GetCamera().SetAffineTransform(affine);
}
