#include "stdafx.h"
#include "GraphicsInterface.h"

#include <windows.h>

#include "Core\GraphicsCore.h"
#include "Core\RenderItem.h"
#include "Core\Lighting.h"
#include "Interface\InternalHandle.h"

using namespace Graphics;
using namespace std;

void grInit(HWND hWnd, grInitParams params) {
    Graphics::InitParams ip;
    ip.SceneObjectsCountLimit = params.SceneObjectsCountLimit;
    ip.PassesCountLimit = params.PassesCountLimit;
    ip.MaterialsCountLimit = params.MaterialsCountLimit;
    ip.FrameResourcesCount = params.FrameResourcesCount;
    GraphicsCore::GetInstance().Initialize(hWnd, ip);
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

grMaterial grCreateMaterial(
    const std::string& name,
    const DirectX::XMFLOAT4& ambient,
    const DirectX::XMFLOAT4& diffuse,
    const DirectX::XMFLOAT4& specular,
    float fresnelR0,
    float roughness)
{
    auto& freeMatCbIndices = GraphicsCore::GetInstance().GetFreeMaterialCbIndices();
    auto cbIndex = freeMatCbIndices.AcquireIndex();
    Material *m = new Material(name, cbIndex);
    m->Update(ambient, diffuse, specular, fresnelR0, roughness);

    return grMaterial(m);
}

grMaterial grCreateStandardMaterial(LibraryMaterial lm, const string& name) {
    unique_ptr<Material> m;
    auto& freeMatCbIndices = GraphicsCore::GetInstance().GetFreeMaterialCbIndices();
    auto cbIndex = freeMatCbIndices.AcquireIndex();
    switch (lm) {
    case LibraryMaterial::kEmerald:
        m = Material::Create(Material::Type::kEmerald, name, cbIndex);
        break;
    case LibraryMaterial::kJade:
        m = Material::Create(Material::Type::kJade, name, cbIndex);
        break;
    case LibraryMaterial::kObsidian:
        m = Material::Create(Material::Type::kObsidian, name, cbIndex);
        break;
    case LibraryMaterial::kSilver:
        m = Material::Create(Material::Type::kSilver, name, cbIndex);
        break;
    case LibraryMaterial::kTurquesa:
        m = Material::Create(Material::Type::kTurquesa, name, cbIndex);
        break;
    }
    return grMaterial(m.release());
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
    const std::vector<grRenderSubItemDesc>& renderItems,
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

void grUpdateRenderSubItemTransform(grRenderItem renderItem, const std::string& name, const XMFLOAT4X4& transform) {
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    auto& subItem = ri->FindSubItem(name);
    subItem.SetTransform(transform);
}

void grUpdateMaterial(
    grMaterial material, 
    const DirectX::XMFLOAT4& ambient,
    const DirectX::XMFLOAT4& diffuse,
    const DirectX::XMFLOAT4& specular,
    float fresnelR0,
    float roughness) 
{
    Material* m = static_cast<MaterialHandle>(material).GetValue();
    m->Update(ambient, diffuse, specular, fresnelR0, roughness);
}

void grDestroyRenderItem(grRenderItem renderItem) {
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    delete ri;
}

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine) {
    GraphicsCore::GetInstance().GetCamera().SetAffineTransform(affine);
}
