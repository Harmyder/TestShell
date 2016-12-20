#pragma once
#include "Interface\GraphicsHandle.h"

namespace Graphics
{
    class RenderItem;
    class CommandContext;

    struct RenderItemHandle : public grRenderItem {
        RenderItemHandle(grRenderItem ri) : grRenderItem(ri) {}
        ~RenderItemHandle();
        RenderItem* GetValue() { return static_cast<RenderItem*>(m_RenderItem); }
    };

    struct CommandContextHandle : public grCommandContext {
        CommandContextHandle(grCommandContext cc) : grCommandContext(cc) {}
        ~CommandContextHandle();
        CommandContext* GetValue() { return static_cast<CommandContext*>(m_CommandContext); }
    };
}
