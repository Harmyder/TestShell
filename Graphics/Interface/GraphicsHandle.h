#pragma once

struct grHandle {
    grHandle(void *handle) : handle_(handle) {}

protected:
    void *handle_;
};

struct grCommandContext : grHandle { grCommandContext(void *commandContext) : grHandle(commandContext) {} }; 

struct grRenderItem : grHandle { grRenderItem(void *renderItem) : grHandle(renderItem) {} };

struct grRenderIndexedItem : grHandle { grRenderIndexedItem(void *renderIndexedItem) : grHandle(renderIndexedItem) {} };

struct grMaterial : grHandle { grMaterial(void *material) : grHandle(material) {} };

struct grDirectionalLight : grHandle { grDirectionalLight(void *light) : grHandle(light) {} };

struct grPointLight : grHandle { grPointLight(void *light) : grHandle(light) {} };

struct grSpotLight : grHandle { grSpotLight(void *light) : grHandle(light) {} };