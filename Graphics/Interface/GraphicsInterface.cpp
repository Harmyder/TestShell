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
    Graphics::InitParams ip(
        params.SceneObjectsCountLimit,
        params.PassesCountLimit,
        params.MaterialsCountLimit,
        params.FrameResourcesCount);
    GraphicsCore::GetInstance().Initialize(hWnd, ip);
}

void grShutdown() {}

void grSetPerspective(float heightOverWidth, float verticalFov, float nearClipPlane, float farClipPlane) {
    auto& camera = GraphicsCore::GetInstance().GetCamera();
    camera.SetPerspective(heightOverWidth, verticalFov, nearClipPlane, farClipPlane);
}

XMMATRIX grGetViewTransform() {
    auto& camera = GraphicsCore::GetInstance().GetCamera();
    return camera.GetViewMatrix();
}

XMMATRIX grGetInvViewTransform() {
    auto& camera = GraphicsCore::GetInstance().GetCamera();
    return camera.GetInvViewMatrix();
}

void grBeginScene() {
    GraphicsCore::GetInstance().BeginScene();
}

void grBeginHud() {
    GraphicsCore::GetInstance().BeginHud();
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
    auto rih = static_cast<RenderItemHandle>(renderItem);
    auto ri = rih.GetValue();
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

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine, XMVECTOR translation) {
    GraphicsCore::GetInstance().GetCamera().SetAffineTransform(affine, translation);
}

XMVECTOR __vectorcall grGetCameraPosition() {
    return GraphicsCore::GetInstance().GetCamera().GetEyePos();
}

grDirectionalLight grCreateDirectionalLight(DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction) {
    auto l = GraphicsCore::GetInstance().GetLightsHolder().CreateDirLightBillet();
    l->Update(strength, direction);
    return grDirectionalLight(l);
}

grPointLight grCreatePointLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation) {
    auto l = GraphicsCore::GetInstance().GetLightsHolder().CreatePntLightBillet();
    l->Update(strength, range, position, attenuation);
    return grPointLight(l);
}

grSpotLight grCreateSpotLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 attenuation) {
    auto l = GraphicsCore::GetInstance().GetLightsHolder().CreateSptLightBillet();
    l->Update(strength, range, position, spot, direction, attenuation);
    return grSpotLight(l);
}

void grUpdateDirectionalLight(grDirectionalLight light, DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction) {
    auto l = static_cast<DirectionalLightHandle>(light).GetValue();
    l->Update(strength, direction);
}

void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation) {
    auto l = static_cast<PointLightHandle>(light).GetValue();
    l->Update(strength, range, position, attenuation);
}

void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 attenuation) {
    auto l = static_cast<SpotLightHandle>(light).GetValue();
    l->Update(strength, range, position, spot, direction, attenuation);
}

void grDestroyDirectionalLight(grDirectionalLight light) {
    auto l = static_cast<DirectionalLightHandle>(light).GetValue();
    GraphicsCore::GetInstance().GetLightsHolder().DestroyLight(l);
}

void grDestroyPointLight(grPointLight light) {
    auto l = static_cast<PointLightHandle>(light).GetValue();
    GraphicsCore::GetInstance().GetLightsHolder().DestroyLight(l);
}

void grDestroySpotLight(grSpotLight light) {
    auto l = static_cast<SpotLightHandle>(light).GetValue();
    GraphicsCore::GetInstance().GetLightsHolder().DestroyLight(l);
}
