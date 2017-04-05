#pragma once
#include "Math/Graph/Graph.h"
#include "Math/Graph/bfs.h"

namespace Common
{
    template <class U>
    class CapacityPassCondition : public Capacity<U> {
    public:
        CapacityPassCondition(Capacity<U> capacity) : Capacity<U>(capacity.GetCapacity()) {}
        bool operator<(const CapacityPassCondition& other) const { return *(Capacity<U>*)this < *(Capacity<U>*)&other; }

        void IncreaseFlow(const U& flow) {
            SetCapacity(GetCapacity() - flow);
            assert(GetCapacity() >= U(0));
        }
        bool IsPassable() const { return GetCapacity() > U(0); }
    };

    // Forbidden for input graph to have opposite edges
    template <class G>
    class EdmondsKarp
    {
        using CapacityUnit = typename G::AdjacencyDatum::EdgeDataType::CapacityUnit;
        using EdgeDataType = CapacityPassCondition<CapacityUnit>;
        using AdjacencyDatum = EdgeData<EdgeDataType>;
        using ResidualNetwork = DirectedGraph<EdgeDataType, VertexData<>>;
    public:
        EdmondsKarp(const G& graph, int source, int sink) : graph_(graph), source_(source), sink_(sink) {
            residualNetwork_.InitVertices(graph_.GetVerticesCount());
            std::vector<std::vector<AdjacencyDatum>> adjacency(graph_.GetVerticesCount());
            for (int i = 0; i < graph_.GetVerticesCount(); i++) {
                const auto adjacencyData = graph_.GetAdjacencyData(i);
                const int adjacentCount = (int)(adjacencyData.second - adjacencyData.first);
                for (int j = 0; j < adjacentCount; ++j) {
                    const int adjacentVertex = GetAdjacentIndex(*(adjacencyData.first + j));
                    adjacency[i].push_back(AdjacencyDatum(adjacentVertex, (adjacencyData.first + j)->edgeData));
                    adjacency[adjacentVertex].push_back(AdjacencyDatum(i, Capacity<CapacityUnit>(0)));
                }
            }
            for (int i = 0; i < graph_.GetVerticesCount(); i++) {
                std::sort(begin(adjacency[i]), end(adjacency[i]));
                const int adjacentCount = (int)(adjacency[i].size());
                std::vector<AdjacencyDatum> adjacencyData; adjacencyData.reserve(adjacentCount);
                residualNetwork_.InitAdjacency(i, move(adjacency[i]));
            }
        }

        void Run() {
            bool done = false;
            while (!done) {
                const auto path = FindAugmentingPath();
                if (path.size() == 0) done = true;
                else {
                    auto pathCapacity = path[0].forward->edgeData.GetCapacity();
                    for (int i = 1; i < (int)path.size(); ++i) {
                        pathCapacity = std::min(pathCapacity, path[i].forward->edgeData.GetCapacity());
                    }
                    for (const auto e : path) {
                        e.forward->edgeData.IncreaseFlow(pathCapacity);
                        e.backward->edgeData.IncreaseFlow(-pathCapacity);
                    }
                }
            }
        }

        std::vector<std::vector<CapacityUnit>> CollectFlow() {
            const int verticesCount = graph_.GetVerticesCount();
            std::vector<std::vector<CapacityUnit>> flow(verticesCount);
            for (int i = 0; i < verticesCount; ++i) {
                const auto flowless = graph_.GetAdjacencyData(i);
                flow[i].reserve(flowless.second - flowless.first);
                const auto flowful = residualNetwork_.GetAdjacencyData(i);
                auto aful = flowful.first;
                for (auto aless = flowless.first; aless < flowless.second; ++aless) {
                    while (aful->indexTo != aless->indexTo) ++aful;
                    flow[i].push_back(aless->edgeData.GetCapacity() - aful->edgeData.GetCapacity());
                }
            }
            return flow;
        }

    private:
        struct TwoWayPair {
            typename ResidualNetwork::AdjacencyDatum* forward;
            typename ResidualNetwork::AdjacencyDatum* backward;
        };
        using TwoWayPath = std::vector<TwoWayPair>;
        TwoWayPath FindAugmentingPath() {
            try {
                const auto previous = bfs(residualNetwork_, source_, sink_, [](const ResidualNetwork::AdjacencyDatum& data) { return data.edgeData.IsPassable(); });
                const auto length = previous[sink_].depth;
                TwoWayPath res(length);
                int prev = sink_;
                for (int i = length - 1; i >= 0; --i) {
                    const int current = prev;
                    prev = previous[current].prev;
                    const auto pf = residualNetwork_.GetAdjacencyDataNonConst(prev);
                    const auto forward = std::find_if(pf.first, pf.second, [&current](const auto& a) { return current == a.indexTo; });
                    const auto pb = residualNetwork_.GetAdjacencyDataNonConst(current);
                    const auto backward = std::find_if(pb.first, pb.second, [&prev](const auto& a) { return prev == a.indexTo; });
                    res[i].forward = &*forward;
                    res[i].backward = &*backward;
                }
                return res;
            }
            catch (const std::logic_error&) { return TwoWayPath(); }
        }

    private:
        G graph_;
        ResidualNetwork residualNetwork_;
        int source_, sink_;
    };
}
