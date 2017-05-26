#include "stdafx.h"
#include "GraphicsInterface.h"

#include <windows.h>

#include "Core\GraphicsCore.h"
#include "Core\RenderItem.h"
#include "Core\RenderItemWithInstances.h"
#include "Core\Lighting.h"
#include "Core\Texture.h"
#include "SDK\PipelineStateObject.h"
#include "SDK\RootSignature.h"
#include "SDK\CommandContext.h"
#include "SDK\CommandQueue.h"
#include "Interface\InternalHandle.h"

using namespace Graphics;
using namespace std;

static_assert(sizeof(RenderVerticesDesc) == sizeof(grtRenderVertices), "Keep RenderVerticesDesc and grtRenderVertices in sync");
static_assert(sizeof(RenderItemDesc) == sizeof(grtRenderSubItemDesc), "Keep RenderVerticesDesc and grtRenderVertices in sync");
static_assert(sizeof(RenderItemInstanceDesc) == sizeof(grtRenderSubItemInstanceDesc), "Keep RenderItemInstanceDesc and grtRenderSubItemInstanceDesc in sync");
static_assert(sizeof(RenderItemWithInstancesDesc) == sizeof(grtRenderSubItemWithInstancesDesc), "Keep RenderItemWithInstancesDesc and grtRenderSubItemWithInstancesDesc in sync");

void grInit(HWND hWnd, grInitParams params) {
    Graphics::InitParams ip(
        params.SceneObjectsCountLimit,
        params.InstancesCountLimit,
        params.PassesCountLimit,
        params.MaterialsCountLimit,
        params.TexturesCountLimit,
        params.ParticlesMetasCountLimit,
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
    auto& mb = GraphicsCore::GetInstance().GetMaterialsBuffer();
    auto m = mb.Create(name, ambient, diffuse, specular, fresnelR0, roughness);
    return grMaterial(m);
}

grMaterial grCreatePredefinedMaterial(greLibraryMaterial::Type lm, const string& name) {
    auto& mb = GraphicsCore::GetInstance().GetMaterialsBuffer();
    auto m = mb.CreatePredefined(name, (Material::Type)(underlying_type_t<Material::Type>)lm);
    return grMaterial(m);
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

void grDestroyMaterial(grMaterial material) {
    Material* m = static_cast<MaterialHandle>(material).GetValue();
    auto& mb = GraphicsCore::GetInstance().GetMaterialsBuffer();
    mb.Destroy(m);
    GraphicsCore::GetInstance().GetCommandQueue()->WaitAllDone();
}

void grSetTexturesRootDirectory(const std::wstring& rootDirectory) {
    TextureCache::Initialize(rootDirectory);
}

grTexture grCreateTextureFromFile(const std::wstring& name) {
    auto& tb = GraphicsCore::GetInstance().GetTexturesBuffer();
    auto t = tb.CreateFromFile(name);
    return grTexture(t);
}

grTexture grCreateTextureFromMemory(const std::wstring& name, const uint8* data, const uint32 size) {
    auto& tb = GraphicsCore::GetInstance().GetTexturesBuffer();
    auto t = tb.CreateFromMemory(name, data, size);
    return grTexture(t);
}

grTexture grCreateTextureFromHandmadeData(const std::wstring& name, uint32 width, uint32 height, greResourceFormat::Type format, const void* data, bool forceRecreation) {
    auto& tb = GraphicsCore::GetInstance().GetTexturesBuffer();
    auto t = tb.CreateFromHandmadeData(name, width, height, *(DXGI_FORMAT*)&format, data, forceRecreation);
    return grTexture(t);
}

void grDestroyTexture(grTexture texture) {
    Texture* t = static_cast<TextureHandle>(texture).GetValue();
    auto& tb = GraphicsCore::GetInstance().GetTexturesBuffer();
    GraphicsCore::GetInstance().GetCommandQueue()->WaitAllDone();
    tb.Destroy(t);
}

void grDrawRenderItem(grRenderItem ri) {
    auto v = static_cast<RenderItemHandle*>(&ri)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItem(*v);
}

void grDrawRenderItem(grRenderItemParticles ri) {
    auto v = static_cast<RenderItemParticlesHandle*>(&ri)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItem(*v);
}

void grDrawRenderItem(grRenderItemWithInstances riwi) {
    auto v = static_cast<RenderItemWithInstancesHandle*>(&riwi)->GetValue();
    GraphicsCore::GetInstance().DrawRenderItemWithInstances(*v);
}


void grDrawRenderSubItem(grRenderItem ri, const string& name) {
    auto v = static_cast<RenderItemHandle*>(&ri)->GetValue();
    GraphicsCore::GetInstance().DrawRenderSubItem(*v, name);
}

grCommandContext grGetGraphicsContext() {
    auto cc = grCommandContext(GraphicsCore::GetInstance().GetCommandContext());
    return cc;
}

grRootSignature grCreateRootSignature(greRootSignature::Type type) {
    auto rs = make_unique<RootSignature>();
    rs->SetType(*(RootSignatureType*)&type);
    rs->Finalize();
    return grRootSignature(rs.release());
}

void grSetRootSignature(grRootSignature rootSignature, grCommandContext cc) {
    CommandContext* ccInternal = static_cast<CommandContextHandle*>(&cc)->GetValue();
    RootSignature* rs = static_cast<RootSignatureHandle>(rootSignature).GetValue();
    ccInternal->SetRootSignature(*rs);
    GraphicsCore::GetInstance().SetGraphicsRoot(rs->GetType());
}

void grDestroyRootSignature(grRootSignature rootSignature) {
    RootSignature* rs = static_cast<RootSignatureHandle>(rootSignature).GetValue();
    delete rs;
}

grPipelineStateObject grCreatePipelineStateObject(const grtPipelineStateDesc& desc, grRootSignature rootSignature) {
    auto pso = make_unique<PipelineStateObject>();
    pso->SetBlendEnable(desc.BlendEnable);
    pso->SetDepthEnable(desc.DepthEnable);
    pso->SetFillMode(*(D3D12_FILL_MODE*)&desc.FillMode);
    pso->SetCullMode(*(D3D12_CULL_MODE*)&desc.CullMode);
    pso->SetPrimitiveTopologyType(*(D3D12_PRIMITIVE_TOPOLOGY_TYPE*)&desc.PrimitiveTolopologyType);
    pso->SetVertexType(*(VertexType*)&desc.VertexType);
    pso->SetShaderType(*(ShaderType*)&desc.ShaderType);
    RootSignature* ri = static_cast<RootSignatureHandle*>(&rootSignature)->GetValue();
    pso->SetRootSignature(*ri);
    pso->Finalize();
    return grPipelineStateObject(pso.release());
}

void grSetPipelineStateObject(grPipelineStateObject pipelineState, grCommandContext cc) {
    CommandContext* ccInternal = static_cast<CommandContextHandle*>(&cc)->GetValue();
    PipelineStateObject* pso = static_cast<PipelineStateHandle>(pipelineState).GetValue();
    ccInternal->SetPipelineStateObject(*pso);
}

void grDestroyPipelineStateObject(grPipelineStateObject pipelineState) {
    PipelineStateObject* pso = static_cast<PipelineStateHandle>(pipelineState).GetValue();
    delete pso;
}

template <class grRI, class D, class RI, class grtD>
grRI CreateRenderItem(const grtD& renderItemDesc, uint32 vertexSize) {
    assert(renderItemDesc.renderVerticesCount && renderItemDesc.renderSubItemsCount);
    auto renderSubItemsInternal = (D*)renderItemDesc.renderSubItems;
    auto renderVerticesInternal = (RenderVerticesDesc*)renderItemDesc.renderVertices;
    unique_ptr<RI> ri;
    RI::Create(
        renderSubItemsInternal, renderItemDesc.renderSubItemsCount,
        renderVerticesInternal, renderItemDesc.renderVerticesCount,
        renderItemDesc.itemsToVertices,
        vertexSize, ri);
    return grRI(ri.release());
}

grRenderItem grCreateRenderItem(const grtRenderItemDesc& renderItemDesc, uint32 vertexSize) {
    return CreateRenderItem<grRenderItem, RenderItemDesc, RenderItem>(renderItemDesc, vertexSize);
}

grRenderItemParticles grCreateRenderItem(const grtRenderItemParticlesDesc& renderItemDesc, uint32 vertexSize) {
    return CreateRenderItem<grRenderItemParticles, RenderItemParticlesDesc, RenderItemParticles>(renderItemDesc, vertexSize);
}

grRenderItemWithInstances grCreateRenderItemWithInstances(const grtRenderSubItemWithInstancesDesc& desc, const grtRenderVertices& vertices, uint32 vertexSize) {
    assert(desc.instancesCount > 0);
    unique_ptr<RenderItemWithInstances> riwi;
    RenderItemWithInstances::Create(*(RenderItemWithInstancesDesc*)&desc, *(RenderVerticesDesc*)&vertices, vertexSize, riwi);
    return grRenderItemWithInstances(riwi.release());
}

void grUpdateRenderSubItemTransform(grRenderItem renderItem, const std::string& name, const XMFLOAT4X3& transform) {
    auto ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    auto& subItem = ri->FindSubItem(name);
    subItem.SetTransform(transform);
}

void grUpdateRenderSubItemTransform(grRenderItemParticles renderItem, const std::string& name, const XMFLOAT4X3& transform) {
    auto ri = static_cast<RenderItemParticlesHandle>(renderItem).GetValue();
    auto& subItem = ri->FindSubItem(name);
    subItem.SetTransform(transform);
}

void grUpdateRenderItemInstancesTransforms(grRenderItemWithInstances renderItem, const XMFLOAT4X3& transform, const XMFLOAT4X3* instancesTransforms) {
    auto ri = static_cast<RenderItemWithInstancesHandle>(renderItem).GetValue();
    ri->SetTransform(transform);
    ri->SetInstancesTransforms(instancesTransforms);
}

void grUpdateRenderSubItemVertexData(grRenderItem renderItem, const std::string& name, const uint8* data) {
    auto ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    ri->SetSubItemVertexData(name, data);
}

void grUpdateRenderSubItemVertexData(grRenderItemParticles renderItem, const std::string& name, const uint8* data) {
    auto ri = static_cast<RenderItemParticlesHandle>(renderItem).GetValue();
    ri->SetSubItemVertexData(name, data);
}

void grDestroyRenderItem(grRenderItem renderItem) {
    GraphicsCore::GetInstance().GetCommandQueue()->WaitAllDone();
    RenderItem* ri = static_cast<RenderItemHandle>(renderItem).GetValue();
    delete ri;
}

void grDestroyRenderItem(grRenderItemParticles renderItem) {
    GraphicsCore::GetInstance().GetCommandQueue()->WaitAllDone();
    RenderItemParticles* ri = static_cast<RenderItemParticlesHandle>(renderItem).GetValue();
    delete ri;
}

void grDestroyRenderItem(grRenderItemWithInstances renderItem) {
    GraphicsCore::GetInstance().GetCommandQueue()->WaitAllDone();
    auto ri = static_cast<RenderItemWithInstancesHandle>(renderItem).GetValue();
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

void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 position) {
    auto l = static_cast<PointLightHandle>(light).GetValue();
    l->Update(position);
}

void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation) {
    auto l = static_cast<PointLightHandle>(light).GetValue();
    l->Update(strength, range, position, attenuation);
}

void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 attenuation) {
    auto l = static_cast<SpotLightHandle>(light).GetValue();
    l->Update(strength, range, position, spot, direction, attenuation);
}

void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) {
    auto l = static_cast<SpotLightHandle>(light).GetValue();
    l->Update(position, direction);
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
