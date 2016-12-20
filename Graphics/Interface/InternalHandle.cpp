#include "stdafx.h"
#include "InternalHandle.h"

#include "Core\RenderItem.h"
#include "SDK\CommandContext.h"

namespace Graphics
{
    RenderItemHandle::~RenderItemHandle() { delete static_cast<RenderItem*>(m_RenderItem); }

    CommandContextHandle::~CommandContextHandle() { delete static_cast<CommandContext*>(m_CommandContext); }
}
