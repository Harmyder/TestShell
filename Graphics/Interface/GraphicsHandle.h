#pragma once

struct grCommandContext
{
    grCommandContext(void *commandContext) : commandContext_(commandContext) {}

protected:
    void *commandContext_;
}; 

struct grRenderItem
{
    grRenderItem(void *renderItem) : renderItem_(renderItem) {}

protected:
    void *renderItem_;
};

struct grRenderIndexedItem
{
    grRenderIndexedItem(void *renderIndexedItem) : renderIndexedItem_(renderIndexedItem) {}

protected:
    void *renderIndexedItem_;
};

struct grMaterial
{
    grMaterial(void *material) : material_(material) {}

protected:
    void *material_;
};