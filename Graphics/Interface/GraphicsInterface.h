#pragma once

#include "Interface/GraphicsHandle.h"
#include "Interface/GraphicsTypes.h"
#include "Common/DirectXInclude.h"

struct grInitParams {
    uint32 SceneObjectsCountLimit;
    uint32 InstancesCountLimit;
    uint32 PassesCountLimit;
    uint32 MaterialsCountLimit;
    uint32 TexturesCountLimit;
    uint32 ParticlesMetasCountLimit;
    uint32 FrameResourcesCount;
};
void grInit(HWND hWnd, grInitParams params);
void grShutdown();

void grSetPerspective(float aspectRatioWidthOverHeight, float fov, float nearClipPlane, float farClipPlane);
XMMATRIX grGetViewTransform();
XMMATRIX grGetInvViewTransform();

void grBeginScene();
void grEndScene();
void grDrawRenderItem(grRenderItem ri);
void grDrawRenderItem(grRenderItemParticles ri);
void grDrawRenderItem(grRenderItemWithInstances riwi);
void grDrawRenderSubItem(grRenderItem ri, const std::string& name);

grCommandContext grGetGraphicsContext();

grRootSignature grCreateRootSignature(greRootSignature::Type type);
void grSetRootSignature(grRootSignature rootSignature, grCommandContext commandContext);
void grDestroyRootSignature(grRootSignature rootSignature);

grPipelineStateObject grCreatePipelineStateObject(const grtPipelineStateDesc& desc, grRootSignature rootSignature);
void grSetPipelineStateObject(grPipelineStateObject pipelineState, grCommandContext commandContext);
void grDestroyPipelineStateObject(grPipelineStateObject pipelineState);

grMaterial grCreateMaterial(const std::string& Name, const DirectX::XMFLOAT4& Ambient, const DirectX::XMFLOAT4& Diffuse, const DirectX::XMFLOAT4& Specular, float FresnelR0, float Roughness);
grMaterial grCreatePredefinedMaterial(greLibraryMaterial::Type lm, const std::string& name);
void grUpdateMaterial(grMaterial material, const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT4& specular, float fresnelR0, float roughness);
void grDestroyMaterial(grMaterial material);

void grSetTexturesRootDirectory(const std::wstring& rootDirectory);
grTexture grCreateTextureFromFile(const std::wstring& name);
grTexture grCreateTextureFromMemory(const std::wstring& name, const uint8* data, const uint32 size);
grTexture grCreateTextureFromHandmadeData(const std::wstring& name, uint32 width, uint32 height, greResourceFormat::Type format, const void* data, bool forceRecreation);
void grDestroyTexture(grTexture material);

grRenderItem grCreateRenderItem(const grtRenderItemDesc& renderItemDesc, uint32 vertexSize);
grRenderItemParticles grCreateRenderItem(const grtRenderItemParticlesDesc& renderItemDesc, uint32 vertexSize);
grRenderItemWithInstances grCreateRenderItemWithInstances(const grtRenderSubItemWithInstancesDesc& desc, const grtRenderVertices& vertices, uint32 vertexSize);
void grUpdateRenderSubItemTransform(grRenderItem renderItem, const std::string& name, const XMFLOAT4X3& transform);
void grUpdateRenderSubItemTransform(grRenderItemParticles renderItem, const std::string& name, const XMFLOAT4X3& transform);
void grUpdateRenderItemInstancesTransforms(grRenderItemWithInstances renderItem, const XMFLOAT4X3& transform, const XMFLOAT4X3* instancesTransforms);
void grUpdateRenderSubItemVertexData(grRenderItem renderItem, const std::string& name, const uint8* data);
void grUpdateRenderSubItemVertexData(grRenderItemParticles renderItem, const std::string& name, const uint8* data);
void grDestroyRenderItem(grRenderItem renderItem);
void grDestroyRenderItem(grRenderItemParticles renderItem);
void grDestroyRenderItem(grRenderItemWithInstances renderItem);

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine);
XMVECTOR __vectorcall grGetCameraPosition();

grDirectionalLight grCreateDirectionalLight(DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction);
grPointLight grCreatePointLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation);
grSpotLight grCreateSpotLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 attenuation);

void grUpdateDirectionalLight(grDirectionalLight light, DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction);
void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 position);
void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation);
void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction);
void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 eirection, DirectX::XMFLOAT3 attenuation);

void grDestroyDirectionalLight(grDirectionalLight light);
void grDestroyPointLight(grPointLight light);
void grDestroySpotLight(grSpotLight light);
