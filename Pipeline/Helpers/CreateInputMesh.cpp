#include "stdafx.h"
#include "CreateInputMesh.h"

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
using namespace Common;
using namespace std;

namespace Pipeline {
    namespace Helpers {
        void CreateBoxInternal(float x, float y, float z, InputMesh& output) {
            auto box = GeometryGenerator::CreateBox(x, y, z);

            auto v = GeometryGenerator::ComputeVertices(box.TrianglesPositions, box.TrianglesTexCoords);
            output.SetVisualVertices(move(v.UniqueVertices));
            output.SetTrianglesVertices(move(v.TrianglesVertices));

            output.SetPositions(box.Positions);
            output.SetNormals(box.Normals);
            output.SetTrianglesPositions(box.TrianglesPositions);
            output.SetTexCoords(box.TexCoords);
            output.SetTrianglesTexCoords(box.TrianglesTexCoords);
        }

        void CreateBox(float x, float y, float z, const OrthogonalTransform& transform, InputMesh& output) {
            CreateBoxInternal(x, y, z, output);
            output.SetTransform(transform);
        }

        void CreateBoxInstanced(float x, float y, float z, const vector<OrthogonalTransform>& transforms, InputMesh& output) {
            CreateBoxInternal(x, y, z, output);
            vector<XMFLOAT4X3> transformsInternal; transformsInternal.reserve(transforms.size());
            for (const auto& t : transforms) transformsInternal.push_back(OrthoToAffine(t).Store4x3());
            output.SetTransforms(move(transformsInternal));
        }

        void CreateGridXyInternal(GeometryGenerator::Geometry& grid, InputMesh& output) {
            auto v = GeometryGenerator::ComputeVertices(grid.TrianglesPositions, grid.TrianglesTexCoords);
            output.SetVisualVertices(move(v.UniqueVertices));
            output.SetTrianglesVertices(move(v.TrianglesVertices));

            output.SetPositions(grid.Positions);
            output.SetNormals(grid.Normals);
            output.SetTrianglesPositions(grid.TrianglesPositions);
            output.SetTexCoords(grid.TexCoords);
            output.SetTrianglesTexCoords(grid.TrianglesTexCoords);
            output.SetTransform(OrthogonalTransform(kIdentity));
        }
    }
}

