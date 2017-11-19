#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common/Geometry/Dcel/Tools.h"
#include <Common/Geometry/GeometryGenerator.h>
#include <Common/Hashes.h>
#include <unordered_set>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common::Dcel;

namespace CommonTest
{
    TEST_CLASS(Tools_Test)
    {
        template<class It>
        void AssertStructure(It bTri, It eTri, const Mesh<typename It::value_type>& mesh) {
            using Id = typename It::value_type;
            Assert::AreEqual((size_t)distance(bTri, eTri) / 3, mesh.faces().size());
            for (size_t f = 0; f < mesh.faces().size(); ++f) {
                const Id e = mesh.faces()[f].getEdge();
                vector<Id> actual; actual.reserve(6);
                Id ei = e;
                for (int i = 0; i < 3; ++i) {
                    actual.push_back(mesh.halfedges()[ei].getOrigin());
                    Assert::IsTrue(mesh.halfedges()[ei].getFace() == (Id)f);
                    ei = mesh.halfedges()[ei].getNext();
                }
                Assert::IsTrue(e == ei);

                copy_n(cbegin(actual), 3, back_inserter(actual));
                auto permute = search(cbegin(actual), cend(actual), bTri + f * 3, bTri + (f + 1) * 3);
                Assert::IsTrue(permute != cend(actual));
            }
        }

        template <class It>
        size_t countOutterEdges(It bTri, It eTri) {
            using Id = It::value_type;
            const Id trianglesCount = (Id)distance(bTri, eTri) / 3;
            unordered_set<pair<Id, Id>, Common::pairhash> edges;
            for (Id t = 0; t < trianglesCount; ++t) {
                for (Id i = 0; i < 3; ++i) {
                    Id vStart = *(bTri + t * 3 + i);
                    Id vEnd = *(bTri + t * 3 + (i + 1) % 3);
                    auto it = edges.find(make_pair(vEnd, vStart));
                    if (it == end(edges)) {
                        edges.insert(make_pair(vStart, vEnd));
                    }
                    else {
                        edges.erase(it);
                    }
                }
            }
            return edges.size();
        }

    public:
        TEST_METHOD(TestTools_Tetrahedron) {
            using Id = uint32;
            array<Id, 4 * 3> triangles = {
                0,1,2,
                0,2,3,
                0,3,1,
                1,3,2
            };
            const auto mesh = Create(cbegin(triangles), cend(triangles), 4,  6);
            AssertStructure(cbegin(triangles), cend(triangles), mesh);
        }

        TEST_METHOD(TestTools_Box) {
            using Id = uint32;
            const Id trianglesCount = 12;
            array<Id, trianglesCount * 3> triangles = {
                // bottom
                0,1,2,
                0,2,3,
                // left
                3,2,4,
                4,2,5,
                // back
                5,2,1,
                5,1,6,
                // right
                6,1,7,
                7,1,0,
                // front
                0,3,7,
                7,3,4,
                // top
                4,6,7,
                4,5,6,
            };

            assert(countOutterEdges(cbegin(triangles), cend(triangles)) == 0);

            const auto mesh = Create(cbegin(triangles), cend(triangles), 8, 18);
            AssertStructure(cbegin(triangles), cend(triangles), mesh);
        }

        TEST_METHOD(TestTools_Quad_CCW) {
            using Id = uint32;
            const Id trianglesCount = 2;
            array<Id, trianglesCount * 3> triangles = {
                0,1,2,
                0,2,3,
            };

            assert(countOutterEdges(cbegin(triangles), cend(triangles)) == 4);

            const auto mesh = Create(cbegin(triangles), cend(triangles), 4, 5);
            AssertStructure(cbegin(triangles), cend(triangles), mesh);
        }

        TEST_METHOD(TestTools_Quad_CW) {
            using Id = uint32;
            const Id trianglesCount = 2;
            array<Id, trianglesCount * 3> triangles = {
                2,1,0,
                2,0,3,
            };

            assert(countOutterEdges(cbegin(triangles), cend(triangles)) == 4);

            const auto mesh = Create(cbegin(triangles), cend(triangles), 4, 5);
            AssertStructure(cbegin(triangles), cend(triangles), mesh);
        }
    };
}