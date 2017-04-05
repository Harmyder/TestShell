#include "stdafx.h"
#include "CppUnitTest.h"
#include "Math/Graph/__utils.h"
#include "Common/Math/Graph/Graph.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(Graph_Test)
    {
    public:
        TEST_METHOD(TestGraph_ConstructionCycle) {
            const int verticesCount = 10;
            const auto graph = CreateGraphCycle(verticesCount);

            Assert::AreEqual(verticesCount, graph.GetVerticesCount());
            for (int i = 0; i < verticesCount; ++i) {
                const auto p = graph.GetAdjacencyData(i);
                Assert::AreEqual(1, (int)(p.second - p.first));
                int next = (i + 1) % verticesCount;
                Assert::AreEqual(next, GetAdjacentIndex(*p.first));
            }
        }

        TEST_METHOD(TestGraph_ConstructionStar) {
            const int verticesCount = 10;
            const auto graph = CreateGraphStar(verticesCount, 0);

            Assert::AreEqual(verticesCount, graph.GetVerticesCount());
            const auto p = graph.GetAdjacencyData(0);
            Assert::AreEqual(verticesCount - 1, (int)(p.second - p.first));
            for (int i = 1; i < verticesCount; ++i) Assert::AreEqual(i, *(p.first + i - 1));
            for (int i = 1; i < verticesCount; ++i) {
                const auto p = graph.GetAdjacencyData(i);
                Assert::AreEqual(1, (int)(p.second - p.first));
                Assert::AreEqual(0, *p.first);
            }
        }

        TEST_METHOD(TestGraph_ConstructionVertexData) {
            class TestVertexData { public: int i; };
            DirectedGraph <void, VertexData<TestVertexData>> graph;
            const int verticesCount = 10;
            std::vector<VertexData<TestVertexData>> v(verticesCount);
            for (int i = 0; i < verticesCount; ++i) v[i].i = i;
            graph.InitVertices(move(v));
            const auto p = graph.GetVerticesData();
            Assert::AreEqual(verticesCount, (int)(p.second - p.first));
            for (int i = 0; i < verticesCount; ++i) {
                Assert::AreEqual(i, (p.first + i)->i);
            }
        }

        TEST_METHOD(TestGraph_ConstructionEdgeData) {
            const vector<vector<uint8>> petersen = {
                { 0, 1, 0, 0, 1, 1, 0, 0, 0, 0 },
                { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
                { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 },
                { 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 },
                { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
                { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
                { 0, 1, 0, 0, 0, 0, 0, 0, 1, 1 },
                { 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
                { 0, 0, 0, 1, 0, 1, 1, 0, 0, 0 },
                { 0, 0, 0, 0, 1, 0, 1, 1, 0, 0 }
            };
            const auto g = CreateGraphFromMatrix(petersen);
            const auto verticesCount = 10;
            Assert::AreEqual(verticesCount, g.GetVerticesCount());
            for (int i = 0; i < verticesCount; ++i) {
                const auto adjacentCount = (int)count(cbegin(petersen[i]), cend(petersen[i]), 1);
                const auto pAdjacencyData = g.GetAdjacencyData(i);
                Assert::AreEqual(adjacentCount, (int)(pAdjacencyData.second - pAdjacencyData.first));
                int currentTo = 0;
                for (int j = 0; j < adjacentCount; ++j, ++currentTo) {
                    auto iter = find(cbegin(petersen[i]) + currentTo, cend(petersen[i]), 1);
                    currentTo = (int)(iter - cbegin(petersen[i]));
                    Assert::AreEqual(currentTo, (pAdjacencyData.first + j)->indexTo);
                    Assert::AreEqual(0, (pAdjacencyData.first + j)->edgeData);
                }
            }
        }

        TEST_METHOD(TestGraph_IterateEdgesData) {
            const vector<vector<uint8>> star = {
                { 0, 1, 1, 1, 1, 1 },
                { 0, 0, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 0, 0 },
                { 0, 0, 0, 0, 0, 0 }
            };
            vector<int> edgesData = { 0, 1, 0, 1, 0 };
            const auto edgeData = [&edgesData](int from, int j) { assert(from == 0); return edgesData[j - 1]; };
            struct Pred {
                bool operator()(const EdgeData<int>& d) { return d.edgeData > 0; };
            };
            const auto g = CreateGraphFromMatrix<Pred, decltype(edgeData)>(star, edgeData);
            const auto verticesCount = (int)star.size();
            const auto p = g.GetAdjacencyData(0);
            auto current = cbegin(edgesData);
            for (auto iter = p.first; iter != p.second; ++iter) {
                current = find(current, cend(edgesData), 1);
                Assert::AreEqual((int)(current + 1 - cbegin(edgesData)), iter->indexTo);
                ++current;
            }
            Assert::AreEqual((int)(cend(edgesData) - cbegin(edgesData)), (int)distance(cbegin(edgesData), current + 1));
        }

        TEST_METHOD(TestGraph_ChangeAdjacencyData) {
            const vector<vector<uint8>> pair = {
                { 0, 1 },
                { 1, 0 }
            };
            auto g = CreateGraphFromMatrix(pair);
            const auto p1 = g.GetAdjacencyDataNonConst(0);
            Assert::AreEqual(0, p1.first->edgeData);
            p1.first->edgeData = 1;
            const auto p2 = g.GetAdjacencyData(0);
            Assert::AreEqual(1, p2.first->edgeData);
        }

        TEST_METHOD(TestGraph_AVCI_NoValidItems) {
            vector<int> v = { 0, 0, 0 };
            struct Pred { bool operator()(int) { return false; } };
            AdjacencyVectorConstIterator<vector<int>, Pred> b(v, begin(v)), e(v, end(v));
            Assert::IsTrue(b == e);
        }

        TEST_METHOD(TestGraph_AVCI) {
            vector<int> v = { 0, 1, 0, 2, 3, 0 };
            struct Pred { bool operator()(int i) { return i != 0; } };
            using Iterator = AdjacencyVectorConstIterator<vector<int>, Pred>;
            Iterator b(v, begin(v)), e(v, end(v));
            auto current = cbegin(v);
            for (auto iter = b; iter != e; ++iter) {
                current = find_if(current, cend(v), Pred());
                Assert::AreEqual(*current, *iter);
                ++current;
            }
            const auto after_last_valid = find_if(crbegin(v), crend(v), Pred());
            Assert::AreEqual((int)distance(after_last_valid, crend(v)), (int)distance(cbegin(v), current));
        }

    };
}