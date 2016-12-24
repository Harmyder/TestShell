#pragma once

#include "Interface\GraphicsHandle.h"
#include "Pile\DirectXInclude.h"

void grInit(HWND hWnd);
void grShutdown();

void grSetPerspective(float aspectRatioWidthOverHeight, float fov, float nearClipPlane, float farClipPlane);

void grBeginScene();
void grEndScene();
void grDrawRenderItem(grRenderItem renderItem);
void grDrawRenderIndexedItem(grRenderIndexedItem renderIndexedItem);

// As context contains CommandList may we need to be able to create several
// of them even in single threaded application
grCommandContext grGetGraphicsContext();

grRenderIndexedItem grCreateRenderIndexedItem(
    const void* vertices,
    const uint_t vertexSize,
    const uint_t verticesCount,
    const uint16* indices,
    const uint_t indicesCount,
    const XMFLOAT4X4& transform,
    grCommandContext commandContext);

grRenderItem grCreateRenderItem(
    const void* vertices, 
    const uint_t vertexSize, 
    const uint_t verticesCount, 
    const XMFLOAT4X4& transform,
    grCommandContext commandContext);

void grDestroyRenderItem(grRenderItem renderItem);

void __vectorcall grSetCameraAffineTransform(FXMMATRIX affine);
