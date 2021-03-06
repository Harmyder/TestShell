#pragma once

struct grHandle {
    grHandle(void *handle) : handle_(handle) {}

protected:
    void *handle_;
};

struct grRootSignature : grHandle { grRootSignature(void *rootSignature) : grHandle(rootSignature) {} };

struct grPipelineStateObject : grHandle { grPipelineStateObject(void *pipelineState) : grHandle(pipelineState) {} };

struct grCommandContext : grHandle { grCommandContext(void *commandContext) : grHandle(commandContext) {} }; 

struct grRenderItem              : grHandle { grRenderItem(void *renderItem)              : grHandle(renderItem) {} };
struct grRenderItemParticles     : grHandle { grRenderItemParticles(void *renderItem)     : grHandle(renderItem) {} };
struct grRenderItemWithInstances : grHandle { grRenderItemWithInstances(void *renderItem) : grHandle(renderItem) {} };

struct grMaterial : grHandle { grMaterial(void *material) : grHandle(material) {} };
extern grMaterial grcMaterialNone;

struct grTexture : grHandle { grTexture(void *texture) : grHandle(texture) {} };
extern grTexture grcTextureNone;

struct grDirectionalLight : grHandle { grDirectionalLight(void *light) : grHandle(light) {} };
struct grPointLight       : grHandle { grPointLight(void *light)       : grHandle(light) {} };
struct grSpotLight        : grHandle { grSpotLight(void *light)        : grHandle(light) {} };