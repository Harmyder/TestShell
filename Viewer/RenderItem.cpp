#include "stdafx.h"
#include "RenderItem.h"

#include "Graphics\GpuBuffer.h"

using namespace Graphics;

namespace Viewer
{
    RenderItem::RenderItem() {}
    RenderItem::~RenderItem() {}

    RenderItem& RenderItem::operator=(RenderItem&& other) {
        indexBuffer_ = move(other.indexBuffer_);
        vertexBuffer_ = move(other.vertexBuffer_);
        return *this;
    }
}
