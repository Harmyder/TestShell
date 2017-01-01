#pragma once

#include "Interface\GraphicsHandle.h"
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

void grBeginScene();
void grBeginHud();
void grEndScene();
void grDrawRenderItem(grRenderItem renderItem);
void grDrawRenderSubItem(grRenderItem renderItem, const std::string& name);

// As context contains CommandList may we need to be able to create several
// of them even in single threaded application
grCommandContext grGetGraphicsContext();

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

enum class LibraryMaterial {
    kRed, kGreen, kBlue,
    kTurquesa,
    kEmerald,
    kJade,
    kObsidian,
    kSilver,
};
grMaterial grCreateStandardMaterial(LibraryMaterial lm, const std::string& name);

struct grRenderVertices {
    grRenderVertices(const uint8* data, const uint32 verticesCount) :
        data(data),
        verticesCount(verticesCount) {}

    const uint8* data;
    const uint32 verticesCount;
};
struct grRenderSubItemDesc
{
    grRenderSubItemDesc(const std::string& name, const XMFLOAT4X4& transform, grMaterial material) :
        name(name),
        transform(transform),
        material(material)
    {}

    const std::string& name;
    const XMFLOAT4X4& transform;
    grMaterial material;
};
grRenderItem grCreateRenderItem(
    const std::vector<grRenderVertices>& vertices,
    const std::vector<grRenderSubItemDesc>& renderItems,
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
