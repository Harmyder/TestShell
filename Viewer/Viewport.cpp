#include "stdafx.h"
#include "Viewport.h"

#include "CameraController.h"
#include "GameInput.h"
#include "GeometryGenerator.h"
#include "Pile\Math\XmFloatHelper.h"
#include "Graphics\Interface\GraphicsInterface.h"
#include "Graphics\Interface\GraphicsConsts.h"

using namespace std;
using namespace DirectX;

namespace Viewer
{
    namespace PrimitiveTopology {
        CHECK_NAMESPACE_ENUM_TYPE(grePrimitiveTopology);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kInvalid);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kTriangleList);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kLineList);
        DEFINE_NAMESPACE_ENUM_TOSRC(grePrimitiveTopology);
    }

    constexpr uint32 kSceneObjectsCountLimit = 100;
    constexpr uint32 kInstancesCountLimit = 1000;
    constexpr uint32 kPassesCountLimit = 1;
    constexpr uint32 kMaterialsCountLimit = 10;
    constexpr uint32 kFrameResourcesCount = 3;

    constexpr auto kNearClipPlane = .3f;
    constexpr auto kFarClipPlane = 200.f;
    const auto kVerticalFov = DirectX::XM_PIDIV4;

    namespace Material {
        CHECK_NAMESPACE_ENUM_TYPE(greLibraryMaterial);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kInvalid);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kRed);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kGreen);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kBlue);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kTurquesa);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kEmerald);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kJade);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kObsidian);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kSilver);
        DEFINE_NAMESPACE_ENUM_TOSRC(greLibraryMaterial);
    }

    Viewport::Viewport(HWND hWnd) : hwnd_(hWnd),
        rootSignature_(nullptr),
        referenceFrame_(nullptr),
        grid_(nullptr)
    {
        grInit(hwnd_, { kSceneObjectsCountLimit, kInstancesCountLimit, kPassesCountLimit, kMaterialsCountLimit, kFrameResourcesCount });
        rootSignature_ = grCreateRootSignature();
        PreparePsos();

        RECT rect;
        GetClientRect(hWnd, &rect);
        width_ = rect.right - rect.left;
        height_ = rect.bottom - rect.top;

        grSetPerspective(static_cast<float>(width_) / height_, kVerticalFov, kNearClipPlane, kFarClipPlane);

        PrepareGeometry();

        grCreateDirectionalLight(XMFLOAT3(.5f, .5f, .45f), XMFLOAT3(1.f, 0.f, 0.f));
        grCreateDirectionalLight(XMFLOAT3(.9f, .9f, .8f), XMFLOAT3(0.f, 0.f, 1.f));
        grCreateDirectionalLight(XMFLOAT3(.3f, .3f, .37f), XMFLOAT3(0.f, 0.f, -1.f));

        CreateMaterial(Material::kRed(), "red");
        CreateMaterial(Material::kGreen(), "green");
        CreateMaterial(Material::kBlue(), "blue");

        vector<RenderItemTypeDesc> descs;
        auto type = PredefinedGeometryType::kCone;
        descs.emplace_back("X", type, Pile::Identity4x4(), "red", PrimitiveTopology::kTriangleList());
        descs.emplace_back("Y", type, Pile::Identity4x4(), "green", PrimitiveTopology::kTriangleList());
        descs.emplace_back("Z", type, Pile::Identity4x4(), "blue", PrimitiveTopology::kTriangleList());
        referenceFrame_ = CreateRenderItemInternal(descs);

        vector<VertexColor> vertices;
        GeometryGenerator::CreateGridXY(vertices, 10, 10, 1.f, 1.f);
        DescsVertices descsGrid;
        XMFLOAT4X4 t; XMStoreFloat4x4(&t, XMMatrixRotationX(XM_PIDIV2));
        RenderItemVerticesDesc descGrid("Grid", (uint8*)vertices.data(), (uint32)vertices.size(), t, "red", PrimitiveTopology::kLineList());
        descsGrid.push_back(descGrid);
        grid_ = CreateRenderItemInternal(descsGrid, sizeof(VertexColor));
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    void Viewport::PreparePsos() {
        grtPipelineStateDesc desc;
        desc.BlendEnable = false;
        desc.DepthEnable = true;
        desc.FillMode = greFillMode::kSolid;
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kTriangle;
        desc.VertexType = greVertexType::kNormalTex;
        psos_.insert(make_pair(PsoType::kOpaque, grCreatePipelineStateObject(desc, rootSignature_)));

        // Transparent objects
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kLine;
        desc.VertexType = greVertexType::kColor;
        psos_.insert(make_pair(PsoType::kLine, grCreatePipelineStateObject(desc, rootSignature_)));

        // Transparent objects
        desc.BlendEnable = true;
        desc.DepthEnable = false;
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kTriangle;
        desc.VertexType = greVertexType::kNormalTex;
        psos_.insert(make_pair(PsoType::kTransparent, grCreatePipelineStateObject(desc, rootSignature_)));
    }

    void Viewport::CreateMaterial(Material::Type material, const string& name) {
        materials_.insert(make_pair(name, grCreatePredefinedMaterial(Material::ToSrc(material), name)));
    }

    void Viewport::BeforeDraw() {
        grPreBeginScene();
        grSetRootSignature(rootSignature_, grGetGraphicsContext());
        grBeginScene();
    }

    void Viewport::AfterDraw() {
        grEndScene();
    }

    void Viewport::BeforeOpaque() { SetPso(PsoType::kOpaque); }
    void Viewport::BeforeLine() { SetPso(PsoType::kLine); }
    void Viewport::BeforeTransparent() { SetPso(PsoType::kTransparent); }

    void Viewport::SetPso(PsoType type) {
        if (currentPsoType_ != type) {
            currentPsoType_ = type;
            grSetPipelineStateObject(psos_.at(currentPsoType_), grGetGraphicsContext());
        }
    }

    void Viewport::DrawRenderItemsOpaque() {
        for (const auto& ri : renderItemsOpaque_) {
            grDrawRenderItem(ri);
        }
    }

    void Viewport::DrawRenderItemsTransparent() {
        for (const auto& ri : renderItemsTransparent_) {
            grDrawRenderItem(ri);
        }
    }

    void Viewport::DrawReferenceFrame() {
        XMVECTOR referenceFrame = Convert2DTo3D(50, 50);
        constexpr float length = .1f;
        constexpr float base = .01f;
        XMMATRIX scaling = XMMatrixScaling(base, length, base);
        XMMATRIX translation = XMMatrixTranslationFromVector(referenceFrame);

        XMMATRIX localTranslation = XMMatrixTranslation(length / 2.f, 0, 0);
        XMMATRIX transform = scaling * XMMatrixRotationZ(-XM_PIDIV2) * translation * localTranslation;
        XMFLOAT4X4 t; XMStoreFloat4x4(&t, transform);
        grUpdateRenderSubItemTransform(referenceFrame_, "X", t);

        localTranslation = XMMatrixTranslation(0, length / 2.f, 0);
        transform = scaling * translation * localTranslation;
        XMStoreFloat4x4(&t, transform);
        grUpdateRenderSubItemTransform(referenceFrame_, "Y", t);

        localTranslation = XMMatrixTranslation(0, 0, length / 2.f);
        transform = scaling * XMMatrixRotationX(XM_PIDIV2) * translation * localTranslation;
        XMStoreFloat4x4(&t, transform);
        grUpdateRenderSubItemTransform(referenceFrame_, "Z", t);

        XMMATRIX view = grGetViewTransform();
        multimap<float, string> order;
        order.insert(make_pair(XMVectorGetZ(XMVector3Transform(g_XMIdentityR0, view)), "X"));
        order.insert(make_pair(XMVectorGetZ(XMVector3Transform(g_XMIdentityR1, view)), "Y"));
        order.insert(make_pair(XMVectorGetZ(XMVector3Transform(g_XMIdentityR2, view)), "Z"));
        
        for (auto& p : order) {
            grDrawRenderSubItem(referenceFrame_, p.second);
        }
    }

    void Viewport::DrawGrid() {
        grDrawRenderItem(grid_);
    }

    void Viewport::PrepareGeometry() {
        GeometryGenerator::CreateCube(geometries_[(size_t)PredefinedGeometryType::kBox]);
        GeometryGenerator::CreateSphere(geometries_[(size_t)PredefinedGeometryType::kSphere], 4);
        GeometryGenerator::CreateCylinder(geometries_[(size_t)PredefinedGeometryType::kCylinder], 0.5f, 0.5f, 1.f, 8, 4);
        GeometryGenerator::CreateCone(geometries_[(size_t)PredefinedGeometryType::kCone], 0.5f, 1.f, 8, 4);
    }

    XMVECTOR Viewport::Convert2DTo3D(uint32 x, uint32 y) const {
        RECT rect;
        GetClientRect(hwnd_, &rect);
        const float width = static_cast<float>(rect.right - rect.left);
        const float height = static_cast<float>(rect.bottom - rect.top);
        const float ratio = width / height;

        const float alpha = kVerticalFov;
        const float tanHalfAlpha = tan(alpha / 2);
        const float tanHalfBeta = ratio * tanHalfAlpha; 

        const float vx = (2.f * x / width - 1.f) * tanHalfBeta;
        const float vy = -(2.f * y / height - 1.f) * tanHalfAlpha;
        const float vz = -1.f;

        XMVECTOR pointInViewSpace = DirectX::XMLoadFloat3(&XMFLOAT3(vx, vy, vz));
        XMMATRIX invViewTransform = grGetInvViewTransform();
        XMVECTOR result = XMVector3Transform(pointInViewSpace, invViewTransform);
        return result;
    }

    RenderItemId Viewport::CreateRenderItemOpaque(const DescsVertices& viewportVerticesDescs, uint32 vertexSize) {
        grRenderItem ri = CreateRenderItemInternal(viewportVerticesDescs, vertexSize);
        renderItemsOpaque_.push_back(ri);
        return --renderItemsOpaque_.end();
    }

    RenderItemId Viewport::CreateRenderItemOpaque(const DescsTypes& viewportTypeDescs) {
        grRenderItem ri = CreateRenderItemInternal(viewportTypeDescs);
        renderItemsOpaque_.push_back(ri);
        return --renderItemsOpaque_.end();
    }

    RenderItemId Viewport::CreateRenderItemTransparent(const DescsVertices& viewportVerticesDescs, uint32 vertexSize) {
        grRenderItem ri = CreateRenderItemInternal(viewportVerticesDescs, vertexSize);
        renderItemsTransparent_.push_back(ri);
        return --renderItemsTransparent_.end();
    }

    RenderItemId Viewport::CreateRenderItemTransparent(const DescsTypes& viewportTypeDescs) {
        grRenderItem ri = CreateRenderItemInternal(viewportTypeDescs);
        renderItemsTransparent_.push_back(ri);
        return --renderItemsTransparent_.end();
    }

    void Viewport::DestroyRenderItemOpaque(const StructRenderItemId& id) {
        grDestroyRenderItem(*id.Value);
        renderItemsOpaque_.erase(id.Value);
    }

    void Viewport::DestroyRenderItemTransparent(const StructRenderItemId& id) {
        grDestroyRenderItem(*id.Value);
        renderItemsTransparent_.erase(id.Value);
    }

    grRenderItem Viewport::CreateRenderItemInternal(const DescsVertices& viewportVerticesDescs, uint32 vertexSize) {
        vector<grtRenderSubItemDesc> descs;
        vector<grtRenderVertices> vertices;
        vector<uint32> itemsToVertices;

        descs.reserve(viewportVerticesDescs.size());
        itemsToVertices.reserve(viewportVerticesDescs.size());
        vertices.reserve(viewportVerticesDescs.size());

        uint32 currentItem = 0;
        for (const auto& d : viewportVerticesDescs) {
            const grtRenderSubItemDesc descEngine(d.name, d.transform, materials_.find(d.material)->second, PrimitiveTopology::ToSrc(d.primitiveTopology));
            descs.push_back(descEngine);
            itemsToVertices.push_back((uint32)vertices.size());
            vertices.emplace_back(d.vertices, (uint32)d.verticesCount);
            ++currentItem;
        }

        return grCreateRenderItem(vertices, descs, itemsToVertices, vertexSize, grGetGraphicsContext());
    }

    grRenderItem Viewport::CreateRenderItemInternal(const DescsTypes& viewportTypeDescs) {
        vector<grtRenderSubItemDesc> descs;
        vector<grtRenderVertices> vertices;
        vector<uint32> itemsToVertices;

        descs.reserve(viewportTypeDescs.size());
        itemsToVertices.reserve(viewportTypeDescs.size());

        uint32 currentItem = 0;
        array<int, (size_t)PredefinedGeometryType::kSize> geometriesIndices;
        enum { kNoIndex = -1 };
        geometriesIndices.fill(kNoIndex);

        for (const auto& d : viewportTypeDescs) {
            uint_t geometryIndex = (uint_t)d.type;
            const grtRenderSubItemDesc descEngine(d.name, d.transform, materials_.find(d.material)->second, PrimitiveTopology::ToSrc(d.primitiveTopology));
            descs.push_back(descEngine);
            const auto& currentGeometry = geometries_[geometryIndex];
            if (geometriesIndices[geometryIndex] == kNoIndex) {
                geometriesIndices[geometryIndex] = (int)vertices.size();
                vertices.emplace_back((uint8*)(void*)currentGeometry.data(), (uint32)currentGeometry.size());
            }
            itemsToVertices.push_back(geometriesIndices[geometryIndex]);
            ++currentItem;
        }

        return grCreateRenderItem(vertices, descs, itemsToVertices, sizeof(VertexNormalTex), grGetGraphicsContext());
    }
}