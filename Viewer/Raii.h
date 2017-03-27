#pragma once

#include "Viewer\Viewport.h"

namespace Viewer
{
    class Raii {
    public:
        static void Init(Viewport *viewport) {
            viewport_ = viewport;
        }

    protected:
        static Viewport* viewport_;
    };

    class MaterialRaii : protected Raii {
    public:
        MaterialRaii(Material material) : material_(material) {}
        ~MaterialRaii() { viewport_->DestroyMaterial(material_); }

        operator Material() const { return material_; }

    private:
        const Material material_;
    };
}