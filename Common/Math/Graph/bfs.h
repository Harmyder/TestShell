#pragma once

#include <queue>
#include "Math/Graph/Graph.h"

namespace Common
{
    namespace { const auto default_edgeValidator = [](const auto&) { return true; }; }

    struct VertexBfsInfo {
        int prev;
        int depth;
    };

    // Returns previous vertex for every vertex and distance from the start
    template <class G, class EdgeValidator = decltype(default_edgeValidator)>
    auto bfs(const G& g, int start, int stop = G::kNoVertex, EdgeValidator edgeValidator = default_edgeValidator) {
        std::queue<int> ready; ready.push(start);
        std::vector<uint8> seen(g.GetVerticesCount(), 0);  seen[start] = 1;
        std::vector<VertexBfsInfo> previous(g.GetVerticesCount(), VertexBfsInfo{ G::kNoVertex, -1 });
        previous[start] = VertexBfsInfo{ G::kNoVertex, 0 };

        int depth = 0;
        while (ready.size() > 0) {
            const int current = ready.front();
            ready.pop();
            const int depth = previous[current].depth + 1;

            const auto p = g.GetAdjacencyData(current);

            for (auto iter = p.first; iter < p.second; ++iter) {
                if (edgeValidator(*iter)) {
                    const int index = GetAdjacentIndex(*iter);
                    if (seen[index] == 0) {
                        seen[index] = 1;
                        previous[index] = VertexBfsInfo{ current, depth };
                        if (index == stop) return previous;
                        ready.push(index);
                    }
                }
            }
        }
        if (stop != G::kNoVertex && previous[stop].prev == G::kNoVertex) throw std::logic_error("Start and stop don't connected");
        return previous;
    }
}
