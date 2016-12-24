#pragma once
#include "Interface\GraphicsHandle.h"

namespace Graphics
{
    class RenderItem;
    class RenderIndexedItem;
    class CommandContext;

    struct RenderItemHandle : public grRenderItem {
        RenderItemHandle(grRenderItem ri) : grRenderItem(ri) {}
        ~RenderItemHandle();
        RenderItem* GetValue() { return static_cast<RenderItem*>(renderItem_); }
    };

    struct RenderIndexedItemHandle : public grRenderIndexedItem {
        RenderIndexedItemHandle(grRenderIndexedItem ri) : grRenderIndexedItem(ri) {}
        ~RenderIndexedItemHandle();
        RenderIndexedItem* GetValue() { return static_cast<RenderIndexedItem*>(renderIndexedItem_); }
    };

    struct CommandContextHandle : public grCommandContext {
        CommandContextHandle(grCommandContext cc) : grCommandContext(cc) {}
        ~CommandContextHandle();
        CommandContext* GetValue() { return static_cast<CommandContext*>(commandContext_); }
    };
}
