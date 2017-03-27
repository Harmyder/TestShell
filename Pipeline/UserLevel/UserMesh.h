#pragma once

#include "Common/Attribute/NonCopyable.h"
#include "Common/Attribute/NamedObject.h"
#include "Common/Math/Vector/Matrix.h"
#include "Viewer/Vertex.h"
#include "UserLevel/IPhysicsData.h"
#include "InputLevel/InputMesh.h"

namespace Pipeline
{
    class InputMesh;

    struct MeshGeometry
    {
        std::vector<Common::Vector3> UniquePositions;
        std::vector<uint16>          TrianglesPositions;
        std::vector<Viewer::VertexNormalTex> UniqueVertices;
        std::vector<uint16>                  TrianglesVertices;
    };

    class UserMesh : public Common::NamedByRefObject, public Common::NonCopyable
    {
    public:
        UserMesh(const InputMesh &mesh);

        void InitPhysicsData(std::unique_ptr<IPhysicsData> physicsData);
        template <class PhysicsData>
        void InitPhysicsDatas(std::vector<std::unique_ptr<PhysicsData>>& physicsDatas) {
            assert(physicsDatas_.size() == 0);
            assert(isInstanced_ == InputMesh::InstanceOption::None);
            isInstanced_ = InputMesh::InstanceOption::Instanced;
            physicsDatas_.reserve(physicsDatas.size());
            for (auto& d : physicsDatas) {
                physicsDatas_.push_back(move(d));
            }
        }

        const auto GetTransform() const { return physicsData_->GetTransform(); }
        const uint32 GetTransformsCount() const;
        const auto GetTransform(uint32 i) const { return physicsDatas_[i]->GetTransform(); }

        const MeshGeometry &GetGeometry() const { return meshGeometry_; }
        MeshGeometry &GetGeometryNonConst() { return meshGeometry_; }
        uint_t GetRenderItemIndex() const { return renderItemIndex_; }
        void SetRenderItemIndex(uint_t index) { renderItemIndex_ = index; }

        const InputMesh& GetInput() const { return input_; }

    private:
        const InputMesh &input_;
        uint_t renderItemIndex_;

        MeshGeometry meshGeometry_;

        InputMesh::InstanceOption isInstanced_ = InputMesh::InstanceOption::None;        
        std::unique_ptr<IPhysicsData> physicsData_;
        std::vector<std::unique_ptr<IPhysicsData>> physicsDatas_;

        Common::Matrix4 transform_;
    };
}
