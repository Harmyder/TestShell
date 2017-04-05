#include "stdafx.h"
#include "CppUnitTest.h"
#include "Math/Graph/__utils.h"
#include "Common/Math/Graph/bfs.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(Bfs_Test)
    {
    public:

        TEST_METHOD(TestBfs_Cycle) {
            const int verticesCount = 10;
            const auto graph = CreateGraphCycle(verticesCount);
            auto previous = bfs(graph, 0);

            Assert::AreEqual(verticesCount, (int)previous.size());
            Assert::AreEqual((int)Graph::kNoVertex, previous[0].prev);
            for (int i = 1; i < verticesCount; ++i) {
                Assert::AreEqual(i - 1, previous[i].prev);
                Assert::AreEqual(i, previous[i].depth);
            }
        }

        TEST_METHOD(TestBfs_FullBinaryTree) {
            const int levels = 10;
            const auto g = CreateGraphBinaryTree(levels);

            const int verticesCount = g.GetVerticesCount();
            auto previous = bfs(g, 0);
            for (int i = 1; i < verticesCount; ++i) {
                const int prev = (i - 1) / 2;
                Assert::AreEqual(prev, previous[i].prev);
                const int depth = (int)log2(i + 1);
                Assert::AreEqual(depth, previous[i].depth);
            }
        }

        TEST_METHOD(TestBfs_NotConnected) {
            DirectedGraph<void, VertexData<>> graph;
            graph.InitVertices(2);
            const auto f = [&graph]() { bfs(graph, 0, 1); };
            Assert::ExpectException<logic_error, decltype(f)>(f);
        }
    };
}