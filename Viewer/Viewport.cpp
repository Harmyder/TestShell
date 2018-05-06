#include "stdafx.h"
#include "Viewport.h"

#include "CameraController.h"
#include "GameInput.h"
#include "GeometryGenerator.h"
#include "Common\Math\Vector\Transform.h"
#include "Graphics\Interface\GraphicsInterface.h"
#include "Graphics\Interface\GraphicsConsts.h"
#include "Raii.h"

using namespace std;
using namespace DirectX;

namespace Viewer
{
    namespace PrimitiveTopology {
        CHECK_NAMESPACE_ENUM_TYPE(grePrimitiveTopology);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kInvalid);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kTriangleList);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kLineList);
        DEFINE_NAMESPACE_ENUM_MEMBER(grePrimitiveTopology, kPointList);
        DEFINE_NAMESPACE_ENUM_TOSRC(grePrimitiveTopology);
    }

    constexpr uint32 kSceneObjectsCountLimit = 10000;
    constexpr uint32 kInstancesCountLimit = 100000;
    constexpr uint32 kPassesCountLimit = 1;
    constexpr uint32 kMaterialsCountLimit = 10;
    constexpr uint32 kTexturesCountLimit = 20;
    constexpr uint32 kParticlesMetasCountLimit = 10;
    constexpr uint32 kFrameResourcesCount = 3;

    constexpr auto kNearClipPlane = .3f;
    constexpr auto kFarClipPlane = 200.f;
    const auto kVerticalFov = DirectX::XM_PIDIV4;

    namespace MaterialType {
        CHECK_NAMESPACE_ENUM_TYPE(greLibraryMaterial);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kInvalid);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kRed);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kGreen);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kBlue);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kWhite);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kTurquesa);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kEmerald);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kJade);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kObsidian);
        DEFINE_NAMESPACE_ENUM_MEMBER(greLibraryMaterial, kSilver);
        DEFINE_NAMESPACE_ENUM_TOSRC(greLibraryMaterial);
    }

    namespace ResourceFormat {
        CHECK_NAMESPACE_ENUM_TYPE(greResourceFormat);
        DEFINE_NAMESPACE_ENUM_MEMBER(greResourceFormat, kInvalid);
        DEFINE_NAMESPACE_ENUM_MEMBER(greResourceFormat, kR8G8B8A8_UNORM);
        DEFINE_NAMESPACE_ENUM_TOSRC(greResourceFormat);
    }
    

    template <class Descs> struct CreateRenderItemTraits;
    template <> struct CreateRenderItemTraits<DescsParticles> {
        using grtRSID = grtRenderSubItemParticlesDesc;
        using grtRID = grtRenderItemParticlesDesc;
        static auto distinctiveParam(const RenderItemParticlesDesc& d) { return d.particleSize; }
        static auto engineCall(const grtRID& rid, uint32 vs) { return grCreateRenderItem(rid, vs); }
    };
    template <> struct CreateRenderItemTraits<DescsVertices> {
        using grtRSID = grtRenderSubItemDesc;
        using grtRID = grtRenderItemDesc;
        static auto distinctiveParam(const RenderItemVerticesDesc& d) { return PrimitiveTopology::ToSrc(d.primitiveTopology); }
        static auto engineCall(const grtRID& rid, uint32 vs) { return grCreateRenderItem(rid, vs); }
    };

    template <class Descs>
    auto Viewport::CreateRenderItemInternal(const Descs& viewportVerticesDescs, uint32 vertexSize) {
        using Traits = CreateRenderItemTraits<Descs>;
        vector<Traits::grtRSID> descs;
        vector<grtRenderVertices> vertices;
        vector<uint32> itemsToVertices;

        descs.reserve(viewportVerticesDescs.size());
        itemsToVertices.reserve(viewportVerticesDescs.size());
        vertices.reserve(viewportVerticesDescs.size());

        uint32 currentItem = 0;
        for (const auto& d : viewportVerticesDescs) {
            const Traits::grtRSID descEngine(d.name, d.transform, d.material.Value, d.texture.Value, Traits::distinctiveParam(d));
            descs.push_back(descEngine);
            itemsToVertices.push_back((uint32)vertices.size());
            vertices.emplace_back(d.vertices, d.verticesCount, d.indices, d.indicesCount);
            ++currentItem;
        }

        Traits::grtRID rid{ vertices.data(), (uint32)vertices.size(), descs.data(), (uint32)descs.size(), itemsToVertices.data() };
        return Traits::engineCall(rid, vertexSize);
    }

    Viewport::Viewport(HWND hWnd) : hwnd_(hWnd),
        referenceFrame_(nullptr),
        grating_(nullptr),
        lightKey_(nullptr), lightFill_(nullptr), lightBack_(nullptr), lightPoint_(nullptr), lightSpot_(nullptr)
    {
        Raii::Init(this);
        grInit(hwnd_, { kSceneObjectsCountLimit, kInstancesCountLimit, kPassesCountLimit, kMaterialsCountLimit, kTexturesCountLimit, kParticlesMetasCountLimit, kFrameResourcesCount });
        PrepareRootSignatures();
        PreparePsos();

        RECT rect;
        GetClientRect(hWnd, &rect);
        width_ = rect.right - rect.left;
        height_ = rect.bottom - rect.top;

        grSetPerspective(static_cast<float>(width_) / height_, kVerticalFov, kNearClipPlane, kFarClipPlane);

        PrepareGeometry();

        lightFill_  = grCreateDirectionalLight(XMFLOAT3(.5f, .5f, .45f), XMFLOAT3(1.f, 0.f, 0.f));
        lightKey_   = grCreateDirectionalLight(XMFLOAT3(.9f, .9f, .8f),  XMFLOAT3(0.f, 0.f, 1.f));
        lightBack_  = grCreateDirectionalLight(XMFLOAT3(.3f, .3f, .37f), XMFLOAT3(0.f, 0.f, -1.f));
        lightPoint_ = grCreatePointLight(XMFLOAT3(.5f, .5f, .5f), 45.f, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, .3f, 1.f));
        lightSpot_  = grCreateSpotLight(XMFLOAT3(1.f, 1.f, 1.f), 50.f, XMFLOAT3(0.f, 0.f, 0.f), 64.f, XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(0.f, .3f, 1.f));

        matRed_ = make_unique<MaterialRaii>(CreateMaterial(MaterialType::kRed(), "red"));
        matGreen_ = make_unique<MaterialRaii>(CreateMaterial(MaterialType::kGreen(), "green"));
        matBlue_ = make_unique<MaterialRaii>(CreateMaterial(MaterialType::kBlue(), "blue"));
        matDummy_ = make_unique<MaterialRaii>(CreateMaterial(MaterialType::kBlue(), "dummy"));

        vector<RenderItemTypeDesc> descs;
        const auto type = PredefinedGeometryType::kCone;
        const auto transform = Common::Matrix4(Common::kIdentity).Store4x3();
        descs.emplace_back("X", type, transform, *matRed_, PrimitiveTopology::kTriangleList());
        descs.emplace_back("Y", type, transform, *matGreen_, PrimitiveTopology::kTriangleList());
        descs.emplace_back("Z", type, transform, *matBlue_, PrimitiveTopology::kTriangleList());
        referenceFrame_ = CreateRenderItemInternal(descs);

        vector<VertexColor> vertices = GeometryGenerator::CreateGratingXY(10, 10, 1.f, 1.f);
        DescsVertices descsGrating;
        XMFLOAT4X3 t; XMStoreFloat4x3(&t, XMMatrixRotationX(XM_PIDIV2));
        RenderItemVerticesDesc descGrating("Grating", (uint8*)vertices.data(), (uint32)vertices.size(), nullptr, 0, t, *matDummy_, Texture(), PrimitiveTopology::kLineList());
        descsGrating.push_back(descGrating);
        grating_ = CreateRenderItemInternal(descsGrating, sizeof(VertexColor));
    }

    Viewport::~Viewport() {
        grShutdown();
    }

    void Viewport::UpdateCamera(const XMFLOAT4X3& transform) {
        XMMATRIX t = XMLoadFloat4x3(&transform);
        grSetCameraAffineTransform(t);

        XMFLOAT3 *p = (XMFLOAT3*)transform.m[3];
        grUpdatePointLight(lightPoint_, *p);
        grUpdateSpotLight(lightSpot_, *p, XMFLOAT3(transform.m[2][0], transform.m[2][1], transform.m[2][2]));
    }

    void Viewport::PrepareRootSignatures() {
        rootSignatures_.insert(make_pair(RootSignatureType::kColor, grCreateRootSignature(greRootSignature::kColor)));
        rootSignatures_.insert(make_pair(RootSignatureType::kLighting, grCreateRootSignature(greRootSignature::kLighting)));
        rootSignatures_.insert(make_pair(RootSignatureType::kLightingWithInstances, grCreateRootSignature(greRootSignature::kLightingWithInstances)));
        rootSignatures_.insert(make_pair(RootSignatureType::kParticles, grCreateRootSignature(greRootSignature::kParticles)));
    }

    void Viewport::PreparePsos() {
        grtPipelineStateDesc desc;
        desc.BlendEnable = false;
        desc.DepthEnable = true;
        desc.FillMode = greFillMode::kWireframe;
        desc.CullMode = greCullMode::kBack;
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kTriangle;
        desc.VertexType = greVertexType::kNormalTex;
        desc.ShaderType = greShaderType::kLighting;
        pso2rs_.insert(make_pair(PsoType::kOpaque, RootSignatureType::kLighting));
        psos_.insert(make_pair(PsoType::kOpaque, grCreatePipelineStateObject(desc, rootSignatures_.at(pso2rs_[PsoType::kOpaque]))));

        desc.ShaderType = greShaderType::kLightingWithInstances;
        pso2rs_.insert(make_pair(PsoType::kOpaqueWithInstances, RootSignatureType::kLightingWithInstances));
        psos_.insert(make_pair(PsoType::kOpaqueWithInstances, grCreatePipelineStateObject(desc, rootSignatures_.at(pso2rs_[PsoType::kOpaqueWithInstances]))));

        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kLine;
        desc.VertexType = greVertexType::kColor;
        desc.ShaderType = greShaderType::kColor;
        pso2rs_.insert(make_pair(PsoType::kLine, RootSignatureType::kColor));
        psos_.insert(make_pair(PsoType::kLine, grCreatePipelineStateObject(desc, rootSignatures_.at(pso2rs_[PsoType::kLine]))));

        desc.BlendEnable = true;
        desc.DepthEnable = false;
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kTriangle;
        desc.VertexType = greVertexType::kNormalTex;
        desc.ShaderType = greShaderType::kLighting;
        pso2rs_.insert(make_pair(PsoType::kTransparent, RootSignatureType::kLighting));
        psos_.insert(make_pair(PsoType::kTransparent, grCreatePipelineStateObject(desc, rootSignatures_.at(pso2rs_[PsoType::kTransparent]))));

        desc.CullMode = greCullMode::kNone;
        desc.PrimitiveTolopologyType = grePrimitiveTopologyType::kPoint;
        desc.VertexType = greVertexType::kParticles;
        desc.ShaderType = greShaderType::kParticles;
        pso2rs_.insert(make_pair(PsoType::kParticles, RootSignatureType::kParticles));
        psos_.insert(make_pair(PsoType::kParticles, grCreatePipelineStateObject(desc, rootSignatures_.at(pso2rs_[PsoType::kParticles]))));
    }

    Material Viewport::CreateMaterial(MaterialType::Type material, const string& name) {
        auto engineMaterial = grCreatePredefinedMaterial(MaterialType::ToSrc(material), name);
        return engineMaterial;
    }

    void Viewport::DestroyMaterial(Material material) {
        grDestroyMaterial(material.Value);
    }

    void Viewport::SetTextureRootDirectory(const wstring& rootDirectory) {
        grSetTexturesRootDirectory(rootDirectory);
    }

    Texture Viewport::CreateTextureFromFile(const wstring& fileTitle) {
        return grCreateTextureFromFile(fileTitle);
    }

    Texture Viewport::CreateTextureFromMemory(const wstring& title, const Common::Dynarray<uint8>& data) {
        return grCreateTextureFromMemory(title, data.data(), (uint32)data.size());
    }

    Texture Viewport::CreateTextureFromHandmadeData(const wstring& title, uint32 width, uint32 height, ResourceFormat::Type format, const void* data, bool forceRecreation) {
        return grCreateTextureFromHandmadeData(title, width, height, ResourceFormat::ToSrc(format), data, forceRecreation);
    }

    void Viewport::DestroyTexture(Texture texture) {
        grDestroyTexture(texture.Value);
    }

    void Viewport::BeforeDraw() {
        grBeginScene();
        grSetRootSignature(rootSignatures_.at(RootSignatureType::kLighting), grGetGraphicsContext());
    }

    void Viewport::AfterDraw() {
        grEndScene();
    }

    void Viewport::BeforeOpaque() { SetPso(PsoType::kOpaque); }
    void Viewport::BeforeOpaqueWithInstances() { SetPso(PsoType::kOpaqueWithInstances); }
    void Viewport::BeforeLine() { SetPso(PsoType::kLine); }
    void Viewport::BeforeTransparent() { SetPso(PsoType::kTransparent); }
    void Viewport::BeforeParticles() { SetPso(PsoType::kParticles); }

    void Viewport::SetRootSignature(RootSignatureType type) {
        if (currentRootSignatureType_ != type) {
            currentRootSignatureType_ = type;
            grSetRootSignature(rootSignatures_.at(type), grGetGraphicsContext());
        }
    }

    void Viewport::SetPso(PsoType type) {
        if (currentPsoType_ != type) {
            currentPsoType_ = type;
            grSetPipelineStateObject(psos_.at(currentPsoType_), grGetGraphicsContext());
            SetRootSignature(pso2rs_[type]);
        }
    }

    void Viewport::DrawRenderItemsOpaque() {
        for (auto& ri : renderItems_[kOpaque]) {
            if (ri.show) grDrawRenderItem(ri.item);
        }
    }

    void Viewport::DrawRenderItemsOpaqueWithInstances() {
        for (auto& riwi : renderItemsWithInstances_) {
            if (riwi.show) grDrawRenderItem(riwi.item);
        }
    }

    void Viewport::DrawRenderItemsTransparent() {
        for (const auto& ri : renderItems_[kTransparent]) {
            if (ri.show) grDrawRenderItem(ri.item);
        }
    }

    void Viewport::DrawRenderItemsParticles() {
        for (const auto& ri : renderItemsParticles_) {
            if (ri.show) grDrawRenderItem(ri.item);
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
        XMFLOAT4X3 t; XMStoreFloat4x3(&t, transform);
        grUpdateRenderSubItemTransform(referenceFrame_, "X", t);

        localTranslation = XMMatrixTranslation(0, length / 2.f, 0);
        transform = scaling * translation * localTranslation;
        XMStoreFloat4x3(&t, transform);
        grUpdateRenderSubItemTransform(referenceFrame_, "Y", t);

        localTranslation = XMMatrixTranslation(0, 0, length / 2.f);
        transform = scaling * XMMatrixRotationX(+XM_PIDIV2) * translation * localTranslation;
        XMStoreFloat4x3(&t, transform);
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

    void Viewport::DrawGrating() {
        grDrawRenderItem(grating_);
    }

    void Viewport::PrepareGeometry() {
        geometries_[(size_t)PredefinedGeometryType::kBox] = move(GeometryGenerator::CreateCube(0.5f));
        geometries_[(size_t)PredefinedGeometryType::kSphere] = move(GeometryGenerator::CreateSphere(3));
        geometries_[(size_t)PredefinedGeometryType::kCylinder] = move(GeometryGenerator::CreateCylinder(0.5f, 0.5f, 1.f, 8, 4));
        geometries_[(size_t)PredefinedGeometryType::kCone] = move(GeometryGenerator::CreateCone(0.5f, 1.f, 8, 4));
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

    StructRenderItemId Viewport::CreateRenderItemOpaque(const DescsVertices& desc, uint32 vertexSize) {
        grRenderItem ri = CreateRenderItemInternal(desc, vertexSize);
        renderItems_[kOpaque].emplace_back(ri);
        return StructRenderItemId(renderItems_[kOpaque].size() - 1, kOpaque);
    }

    StructRenderItemId Viewport::CreateRenderItemOpaque(const DescsTypes& desc) {
        grRenderItem ri = CreateRenderItemInternal(desc);
        renderItems_[kOpaque].emplace_back(ri);
        return StructRenderItemId(renderItems_[kOpaque].size() - 1, kOpaque);
    }

    StructRenderItemId Viewport::CreateRenderItemTransparent(const DescsVertices& desc, uint32 vertexSize) {
        grRenderItem ri = CreateRenderItemInternal(desc, vertexSize);
        renderItems_[kTransparent].emplace_back(ri);
        return StructRenderItemId(renderItems_[kTransparent].size() - 1, kTransparent);
    }

    StructRenderItemId Viewport::CreateRenderItemTransparent(const DescsTypes& desc) {
        grRenderItem ri = CreateRenderItemInternal(desc);
        renderItems_[kTransparent].emplace_back(ri);
        return StructRenderItemId(renderItems_[kTransparent].size() - 1, kTransparent);
    }

    StructRenderItemParticlesId Viewport::CreateRenderItemParticles(const DescsParticles& desc, uint32 vertexSize) {
        grRenderItemParticles ri = CreateRenderItemInternal(desc, vertexSize);
        renderItemsParticles_.emplace_back(ri);
        return StructRenderItemParticlesId(renderItemsParticles_.size() - 1);
    }

    StructRenderItemWithInstancesId Viewport::CreateRenderItemOpaqueWithInstances(const RenderItemWithInstancesDesc& descs, uint32 vertexSize) {
        grRenderItemWithInstances ri = CreateRenderItemInternal(descs, vertexSize);
        renderItemsWithInstances_.emplace_back(ri);
        return StructRenderItemWithInstancesId(renderItemsWithInstances_.size() - 1);
    }

    template <class grRi>
    void DestroyRenderItemInternal(vector<RenderItemInfo<grRi>>& c, size_t i) {
        grDestroyRenderItem(c[i].item);
        c[i] = c.back();
        c.pop_back();
    }

    void Viewport::DestroyRenderItem(const StructRenderItemId& id)              { DestroyRenderItemInternal(renderItems_[id.Type], id.Id); }
    void Viewport::DestroyRenderItem(const StructRenderItemParticlesId& id)     { DestroyRenderItemInternal(renderItemsParticles_, id.Id); }
    void Viewport::DestroyRenderItem(const StructRenderItemWithInstancesId& id) { DestroyRenderItemInternal(renderItemsWithInstances_, id.Id); }

    void Viewport::UpdateRenderSubItemTransform(const StructRenderItemId& id, const string& name, const XMFLOAT4X3& transform) {
        grUpdateRenderSubItemTransform(renderItems_[id.Type][id.Id].item, name, transform);
    }

    void Viewport::UpdateRenderSubItemTransform(const StructRenderItemParticlesId& id, const string& name, const XMFLOAT4X3& transform) {
        grUpdateRenderSubItemTransform(renderItemsParticles_[id.Id].item, name, transform);
    }

    void Viewport::UpdateRenderWithInstancesTransforms(const StructRenderItemWithInstancesId& id, const XMFLOAT4X3& transform, const XMFLOAT4X3* instancesTransforms) {
        grUpdateRenderItemInstancesTransforms(renderItemsWithInstances_[id.Id].item, transform, instancesTransforms);
    }

    void Viewport::UpdateRenderSubItemVertexData(const StructRenderItemId& id, const std::string& name, const uint8* data) {
        grUpdateRenderSubItemVertexData(renderItems_[id.Type][id.Id].item, name, data);
    }

    void Viewport::UpdateRenderSubItemVertexData(const StructRenderItemParticlesId& id, const std::string& name, const uint8* data) {
        grUpdateRenderSubItemVertexData(renderItemsParticles_[id.Id].item, name, data);
    }

    void Viewport::ShowRenderItem(const StructRenderItemId& id, bool show) {
        renderItems_[id.Type][id.Id].show = show;
    }

    void Viewport::ShowRenderItemParticles(const StructRenderItemParticlesId& id, bool show) {
        renderItemsParticles_[id.Id].show = show;
    }

    void Viewport::ShowRenderItemOpaqueWithInstances(const StructRenderItemWithInstancesId& id, bool show) {
        renderItemsWithInstances_[id.Id].show = show;
    }

    grRenderItemWithInstances Viewport::CreateRenderItemInternal(const RenderItemWithInstancesDesc& desc, uint32 vertexSize) {
        grtRenderSubItemWithInstancesDesc engineDesc(
            desc.name, desc.transform, PrimitiveTopology::ToSrc(desc.primitiveTopology), desc.texture.Value,
            (grtRenderSubItemInstanceDesc*)desc.instances.get(), desc.instancesCount);
        grtRenderVertices vertices(desc.vertices, desc.verticesCount, desc.indices, desc.indicesCount);
        return grCreateRenderItemWithInstances(engineDesc, vertices, vertexSize);
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
            const grtRenderSubItemDesc descEngine(d.name, d.transform, d.material.Value, d.texture.Value, PrimitiveTopology::ToSrc(d.primitiveTopology));
            descs.push_back(descEngine);
            const auto& currentGeometry = geometries_[geometryIndex];
            if (geometriesIndices[geometryIndex] == kNoIndex) {
                geometriesIndices[geometryIndex] = (int)vertices.size();
                vertices.emplace_back((uint8*)(void*)currentGeometry.first.data(), (uint32)currentGeometry.first.size(),
                    (uint8*)(void*)currentGeometry.second.data(), (uint32)currentGeometry.second.size());
            }
            itemsToVertices.push_back(geometriesIndices[geometryIndex]);
            ++currentItem;
        }

        grtRenderItemDesc rid{ vertices.data(), (uint32)vertices.size(), descs.data(), (uint32)descs.size(), itemsToVertices.data() };
        return grCreateRenderItem(rid, sizeof(VertexNormalTex));
    }
}
