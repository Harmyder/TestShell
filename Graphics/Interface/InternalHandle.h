#pragma once
#include "Interface\GraphicsHandle.h"

namespace Graphics
{
    class RootSignature;
    class PipelineStateObject;
    class RenderItem;
    class RenderItemParticles;
    class RenderItemWithInstances;
    class CommandContext;
    class Material;
    class DirectionalLight;
    class SpotLight;
    class PointLight;
    class Texture;

    template <class C, class H>
    struct InternalHandle : H {
        InternalHandle(H ri) : H(ri) {}
        C* GetValue() { return static_cast<C*>(handle_); }
    };

    struct RootSignatureHandle          : InternalHandle<RootSignature, grRootSignature>                    { using InternalHandle::InternalHandle; };
    struct PipelineStateHandle          : InternalHandle<PipelineStateObject, grPipelineStateObject>        { using InternalHandle::InternalHandle; };
    struct RenderItemHandle             : InternalHandle<RenderItem, grRenderItem>                          { using InternalHandle::InternalHandle; };
    struct RenderItemParticlesHandle    : InternalHandle<RenderItemParticles, grRenderItemParticles>        { using InternalHandle::InternalHandle; };
    struct RenderItemWithInstancesHandle: InternalHandle<RenderItemWithInstances, grRenderItemWithInstances>{ using InternalHandle::InternalHandle; };
    struct CommandContextHandle         : InternalHandle<CommandContext, grCommandContext>                  { using InternalHandle::InternalHandle; };
    struct MaterialHandle               : InternalHandle<Material, grMaterial>                              { using InternalHandle::InternalHandle; };
    struct DirectionalLightHandle       : InternalHandle<DirectionalLight, grDirectionalLight>              { using InternalHandle::InternalHandle; };
    struct SpotLightHandle              : InternalHandle<SpotLight, grSpotLight>                            { using InternalHandle::InternalHandle; };
    struct PointLightHandle             : InternalHandle<PointLight, grPointLight>                          { using InternalHandle::InternalHandle; };
    struct TextureHandle                : InternalHandle<Texture, grTexture>                                { using InternalHandle::InternalHandle; };
}
