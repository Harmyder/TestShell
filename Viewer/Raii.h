#pragma once

#include "Viewer\Viewport.h"

namespace Viewer
{
    class Raii {
    public:
        static void Init(Viewport *viewport) {
            viewport_ = viewport;
        }
        virtual ~Raii() = 0;

    protected:
        static Viewport* viewport_;
    };
    inline Raii::~Raii() {}

    class MaterialRaii : protected Raii {
    public:
        MaterialRaii(Material material) : material_(material) {}
        ~MaterialRaii() { viewport_->DestroyMaterial(material_); }
        operator Material() const { return material_; }
    private:
        const Material material_;
    };

    class TextureRaii : protected Raii {
    public:
        TextureRaii(Texture texture) : texture_(texture) {}
        ~TextureRaii() { viewport_->DestroyTexture(texture_); }
        operator Texture() const { return texture_; }
    private:
        const Texture texture_;
    };

    class RenderItemRaii : protected Raii {
    public:
        RenderItemRaii(StructRenderItemId riid) : riid_(riid) {}
        operator StructRenderItemId() const { return riid_; }
    protected:
        const StructRenderItemId riid_;
    };

    class RenderItemOpaqueRaii : public RenderItemRaii {
    public:
        using RenderItemRaii::RenderItemRaii;
        ~RenderItemOpaqueRaii() { viewport_->DestroyRenderItemOpaque(riid_); }
    };

    class RenderItemTransparentRaii : public RenderItemRaii {
    public:
        using RenderItemRaii::RenderItemRaii;
        ~RenderItemTransparentRaii() { viewport_->DestroyRenderItemTransparent(riid_); }
    };

    class RenderItemWithInstancesRaii : protected Raii {
    public:
        RenderItemWithInstancesRaii(StructRenderItemWithInstancesId riwiid) : riwiid_(riwiid) {}
        ~RenderItemWithInstancesRaii() { viewport_->DestroyRenderItemOpaqueWithInstances(riwiid_); }
        operator StructRenderItemWithInstancesId() const { return riwiid_; }
    protected:
        const StructRenderItemWithInstancesId riwiid_;
    };
}