#pragma once

struct grCommandContext
{
    grCommandContext(void *commandContext) : m_CommandContext(commandContext) {}

protected:
    void *m_CommandContext;
}; 

struct grRenderItem
{
    grRenderItem(void *renderItem) : m_RenderItem(renderItem) {}

protected:
    void *m_RenderItem;
};
