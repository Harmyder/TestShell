#pragma once

namespace Pipeline {
    struct MeshGeometry;
}

namespace Graphics {
    class GpuBuffer;
}

namespace Viewer
{
    struct Vertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT4 Color;
    };

    class RenderItem
    {
        friend class Viewport;

    public:
        RenderItem();
        ~RenderItem();
        RenderItem& operator=(RenderItem&& other);

    private:
        std::unique_ptr<Graphics::GpuBuffer> vertexBuffer_;
        std::unique_ptr<Graphics::GpuBuffer> indexBuffer_;
        
    };
}
