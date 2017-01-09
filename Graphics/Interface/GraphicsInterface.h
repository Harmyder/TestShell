#pragma once

#include "Interface\GraphicsHandle.h"
#include "GraphicsTypes.h"
#include "Pile\DirectXInclude.h"

struct grInitParams {
    uint32 SceneObjectsCountLimit;
    uint32 PassesCountLimit;
    uint32 MaterialsCountLimit;
    uint32 FrameResourcesCount;
};
void grInit(HWND hWnd, grInitParams params);
void grShutdown();

void grSetPerspective(float aspectRatioWidthOverHeight, float fov, float nearClipPlane, float farClipPlane);
XMMATRIX grGetViewTransform();
XMMATRIX grGetInvViewTransform();

void grPreBeginScene();
void grBeginScene();
void grEndScene();
void grDrawRenderItem(grRenderItem renderItem);
void grDrawRenderSubItem(grRenderItem renderItem, const std::string& name);

// As context contains CommandList may we need to be able to create several
// of them even in single threaded application
grCommandContext grGetGraphicsContext();

grRootSignature grCreateRootSignature();
void grSetRootSignature(grRootSignature rootSignature, grCommandContext commandContext);
void grDestroyRootSignature(grRootSignature rootSignature);

grPipelineStateObject grCreatePipelineStateObject(const grtPipelineStateDesc& desc, grRootSignature rootSignature);
void grSetPipelineStateObject(grPipelineStateObject pipelineState, grCommandContext commandContext);
void grDestroyPipelineStateObject(grPipelineStateObject pipelineState);

grMaterial grCreateMaterial(
    const std::string& Name,
    const DirectX::XMFLOAT4& Ambient,
    const DirectX::XMFLOAT4& Diffuse,
    const DirectX::XMFLOAT4& Specular,
    float FresnelR0,
    float Roughness);
void grUpdateMaterial(
    grMaterial material,
    const DirectX::XMFLOAT4& ambient,
    const DirectX::XMFLOAT4& diffuse,
    const DirectX::XMFLOAT4& specular,
    float fresnelR0,
    float roughness);

grMaterial grCreateStandardMaterial(greLibraryMaterial lm, const std::string& name);

grRenderItem grCreateRenderItem(
    const std::vector<grtRenderVertices>& vertices,
    const std::vector<grtRenderSubItemDesc>& renderItems,
    const std::vector<uint32>& itemsToVertices,
    uint32 vertexSize,
    grCommandContext commandContext);

void grUpdateRenderSubItemTransform(grRenderItem renderItem, const std::string& name, const XMFLOAT4X4& transform);

void grDestroyRenderItem(grRenderItem renderItem);

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine, XMVECTOR translation);
XMVECTOR __vectorcall grGetCameraPosition();

grDirectionalLight grCreateDirectionalLight(DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction);
grPointLight grCreatePointLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation);
grSpotLight grCreateSpotLight(DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 eirection, DirectX::XMFLOAT3 attenuation);

void grUpdateDirectionalLight(grDirectionalLight light, DirectX::XMFLOAT3 strength, DirectX::XMFLOAT3 direction);
void grUpdatePointLight(grPointLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 attenuation);
void grUpdateSpotLight(grSpotLight light, DirectX::XMFLOAT3 strength, float range, DirectX::XMFLOAT3 position, float spot, DirectX::XMFLOAT3 eirection, DirectX::XMFLOAT3 attenuation);

void grDestroyDirectionalLight(grDirectionalLight light);
void grDestroyPointLight(grPointLight light);
void grDestroySpotLight(grSpotLight light);
