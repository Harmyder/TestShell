#pragma once

struct grCommandContext
{
    grCommandContext(void *commandContext) : m_CommandContext(commandContext) {}
    virtual ~grCommandContext() {}

protected:
    void *m_CommandContext;
}; 

struct grRenderItem
{
    grRenderItem(void *renderItem) : m_RenderItem(renderItem) {}
    virtual ~grRenderItem() {}

protected:
    void *m_RenderItem;
};
