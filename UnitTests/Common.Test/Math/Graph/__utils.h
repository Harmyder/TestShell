#pragma once
#include "Common/Math/Graph/Graph.h"
using namespace Common;

namespace CommonTest
{
    namespace {
        const auto default_edgeData = [](int i, int j) { return 0; };

        struct DefaultEdgePred {
            bool operator()(const EdgeData<result_of<decltype(default_edgeData)(int, int)>::type>&) { return true; };
        };

    }

#define DEFINE_CREATE_GRAPH(name, params) \
    template <class FunctorEP = DefaultEdgePred, class FunED = decltype(default_edgeData)> \
    inline auto CreateGraph##name(params, FunED edgeData = default_edgeData)

#define DEFINE_TYPES using EdgeDataType = result_of<FunED(int, int)>::type; \
                     using AdjacencyDatum = EdgeData<EdgeDataType>
#define GRAPH DirectedGraph<EdgeDataType, VertexData<>, AdjacencyVectorIterator<vector<AdjacencyDatum>, FunctorEP>, AdjacencyVectorConstIterator<vector<AdjacencyDatum>, FunctorEP>>

    DEFINE_CREATE_GRAPH(Tree, int verticesCount) {
        DEFINE_TYPES; GRAPH graph;
        graph.InitVertices(verticesCount);
        for (int i = 0; i < verticesCount; ++i) {
            const int next = (i + 1) % verticesCount;
            graph.InitAdjacency(i, { AdjacencyDatum(next, edgeData(i, next)) });
        }
        return graph;
    }

    DEFINE_CREATE_GRAPH(Cycle, int verticesCount) {
        DEFINE_TYPES; GRAPH graph;
        graph.InitVertices(verticesCount);
        for (int i = 0; i < verticesCount; ++i) {
            const int next = (i + 1) % verticesCount;
            graph.InitAdjacency(i, { AdjacencyDatum(next, edgeData(i, next)) });
        }
        return graph;
    }

    DEFINE_CREATE_GRAPH(BinaryTree, uint32 levelsCount) {
        DEFINE_TYPES; GRAPH graph;
        const int verticesCount = (1 << levelsCount) - 1;
        graph.InitVertices(verticesCount);
        for (int i = 0; i < verticesCount / 2; ++i) {
            const int left = i * 2 + 1; const int right = i * 2 + 2;
            graph.InitAdjacency(i, {
                AdjacencyDatum(left, edgeData(i, left)),
                AdjacencyDatum(right, edgeData(i, right)),
            });
        }
        return graph;
    }

    inline auto CreateGraphStar(int verticesCount, int center) {
        DirectedGraph<void, VertexData<>> graph;
        graph.InitVertices(verticesCount);

        vector<int> indices; indices.reserve(verticesCount - 1);
        for (int i = 0; i < verticesCount; ++i) if (i != center) indices.push_back(i);
        graph.InitAdjacency(0, move(indices));

        for (int i = 1; i < verticesCount; ++i) graph.InitAdjacency(i, { 0 });

        return graph;
    }

    DEFINE_CREATE_GRAPH(FromMatrix, const vector<vector<uint8>>& matrix) {
        const int verticesCount = (int)matrix.size();
        DEFINE_TYPES; GRAPH graph;
        graph.InitVertices(verticesCount);
        for (int v = 0; v < verticesCount; ++v) {
            vector<AdjacencyDatum> edgesData;
            assert(matrix[v].size() == matrix.size());
            for (int j = 0; j < verticesCount; ++j) {
                if (matrix[v][j] == 1) edgesData.push_back(AdjacencyDatum(j, edgeData(v, j)));
            }
            graph.InitAdjacency(v, move(edgesData));
        }
        return graph;
    }
}