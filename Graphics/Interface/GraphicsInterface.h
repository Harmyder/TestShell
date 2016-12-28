#pragma once

#include "Interface\GraphicsHandle.h"
#include "Pile\DirectXInclude.h"

void grInit(HWND hWnd);
void grShutdown();

void grSetPerspective(float aspectRatioWidthOverHeight, float fov, float nearClipPlane, float farClipPlane);

void grBeginScene();
void grEndScene();
void grDrawRenderItem(grRenderItem renderItem);
//void grDrawRenderIndexedItem(grRenderIndexedItem renderIndexedItem);

// As context contains CommandList may we need to be able to create several
// of them even in single threaded application
grCommandContext grGetGraphicsContext();

struct grRenderVertices {
    grRenderVertices(const uint8* data, const uint32 verticesCount) :
        data(data),
        verticesCount(verticesCount) {}

    const uint8* data;
    const uint32 verticesCount;
};

struct grRenderItemDesc
{
    grRenderItemDesc(const std::string& name, const XMFLOAT4X4& transform) :
        name(name),
        transform(transform) {}

    const std::string& name;
    const XMFLOAT4X4& transform;
};
grRenderItem grCreateRenderItem(
    const std::vector<grRenderVertices>& vertices,
    const std::vector<grRenderItemDesc>& renderItems,
    const std::vector<uint32>& itemsToVertices,
    uint32 vertexSize,
    grCommandContext commandContext);

void grDestroyRenderItem(grRenderItem renderItem);

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine);
