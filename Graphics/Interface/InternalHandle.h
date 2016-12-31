#pragma once
#include "Interface\GraphicsHandle.h"

namespace Graphics
{
    class RenderItem;
    class RenderIndexedItem;
    class CommandContext;
    class Material;
    class DirectionalLight;
    class SpotLight;
    class PointLight;

    template <class C, class H>
    struct InternalHandle : H {
        InternalHandle(H ri) : H(ri) {}
        C* GetValue() { return static_cast<C*>(handle_); }
    };

    struct RenderItemHandle : InternalHandle<RenderItem, grRenderItem> { using InternalHandle::InternalHandle; };

    struct RenderIndexedItemHandle : InternalHandle<RenderIndexedItem, grRenderIndexedItem> { using InternalHandle::InternalHandle; };

    struct CommandContextHandle : InternalHandle<CommandContext, grCommandContext> { using InternalHandle::InternalHandle; };

    struct MaterialHandle : InternalHandle<Material, grMaterial> { using InternalHandle::InternalHandle; };

    struct DirectionalLightHandle : InternalHandle<DirectionalLight, grDirectionalLight> { using InternalHandle::InternalHandle; };

    struct SpotLightHandle : InternalHandle<SpotLight, grSpotLight> { using InternalHandle::InternalHandle; };

    struct PointLightHandle : InternalHandle<PointLight, grPointLight> { using InternalHandle::InternalHandle; };
}
