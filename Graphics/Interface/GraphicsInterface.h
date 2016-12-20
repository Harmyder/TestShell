#pragma once

#include "Interface\GraphicsHandle.h"

void grInit(HWND hWnd);
void grShutdown();

void grBeginScene();
void grEndScene();
void grDrawRenderItem(grRenderItem renderItem);

// As context contains CommandList may we need to be able to create several
// of them even in single threaded application
grCommandContext grGetGraphicsContext();

grRenderItem grCreateRenderItem(const void* vertices, const uint_t vertexSize, const uint_t verticesCount,
    const uint16* indices, const uint_t indicesCount, grCommandContext commandContext);

void grDestroyRenderItem(grRenderItem renderItem);
