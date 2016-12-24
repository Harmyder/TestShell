#include "stdafx.h"
#include "InternalHandle.h"

#include "Core\RenderItem.h"
#include "SDK\CommandContext.h"

namespace Graphics
{
    RenderItemHandle::~RenderItemHandle() { delete static_cast<RenderItem*>(renderItem_); }
    
    RenderIndexedItemHandle::~RenderIndexedItemHandle() { delete static_cast<RenderItem*>(renderIndexedItem_); }

    CommandContextHandle::~CommandContextHandle() { delete static_cast<CommandContext*>(commandContext_); }
}
