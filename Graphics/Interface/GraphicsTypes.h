#pragma once
#include "Interface\GraphicsConsts.h"

enum class greLibraryMaterial {
    kRed, kGreen, kBlue,
    kTurquesa,
    kEmerald,
    kJade,
    kObsidian,
    kSilver,
    kSize,
};

enum class greVertexType {
    kNormalTex,
    kColor,
    kSize,
};

struct grtPipelineStateDesc {
    greVertexType VertexType;
    bool BlendEnable;
    bool DepthEnable;
    grePrimitiveTopologyType::Type PrimitiveTolopologyType;
    greFillMode::Type FillMode;
};

struct grtVertexNormalTex {
    float Position[3];
    float Normal[3];
    float TexCoord[2];
};
struct grtVertexColor {
    float Position[3];
    float Color[4];
};
struct grtRenderVertices {
    grtRenderVertices(const uint8* data, const uint32 verticesCount) :
        data(data),
        verticesCount(verticesCount) {}

    const uint8* data;
    const uint32 verticesCount;
};
struct grtRenderSubItemDesc
{
    grtRenderSubItemDesc(const std::string& name, const XMFLOAT4X4& transform, grMaterial material, grePrimitiveTopology::Type primitiveTopology) :
        name(name),
        transform(transform),
        material(material),
        primitiveTopology(primitiveTopology)
    {}

    const std::string& name;
    const XMFLOAT4X4& transform;
    grMaterial material;
    grePrimitiveTopology::Type primitiveTopology;
};
