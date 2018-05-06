#pragma once
#include <Geometry/Dcel/Mesh.h>
#include <Container/Dynarray.h>
#include <Print/DebugPrint.h>
#include <unordered_set>
#include <Hashes.h>

namespace Common
{
    namespace Dcel
    {
        template <class It, class F>
        void iterateEdges(It triBegin, const typename It::value_type trianglesCount, F handler) {
            using Id = typename It::value_type;
            for (Id t = 0; t < trianglesCount; ++t) {
                for (Id i = 0; i < 3; ++i) {
                    const Id vStart = *(triBegin + (t * 3 + i));
                    const Id vEnd = *(triBegin + (t * 3 + (i + 1) % 3));
                    handler(t, i, vStart, vEnd);
                }
            }
        }

        template <class Id>
        void handleEdge(Mesh<Id>& mesh, Id edgeIndex, Id faceIndex, Id vStart) {
            auto& halfedge = mesh.halfedges()[edgeIndex];
            halfedge.setFace(faceIndex);
            halfedge.setOrigin(vStart);
            mesh.faces()[faceIndex].setEdge(edgeIndex);
            mesh.vertices()[vStart].setLeaving(edgeIndex);
        }

        template <class It>
        Mesh<typename It::value_type> Create(It triBegin, It triEnd, const typename It::value_type verticesCount, const typename It::value_type edgesCount) {
            using Id = typename It::value_type;
            assert(distance(triBegin, triEnd) % 3 == 0);
            const Id trianglesCount = static_cast<Id>(distance(triBegin, triEnd) / 3);
            Mesh<Id> mesh(verticesCount, edgesCount, trianglesCount);

            std::unordered_set<std::pair<Id, Id>, Common::pairhash> boundaryEdges;
            iterateEdges(triBegin, trianglesCount, [&boundaryEdges](const Id t, const Id i, const Id vStart, const Id vEnd) {
                auto it = boundaryEdges.find(make_pair(vEnd, vStart));
                if (it == end(boundaryEdges)) {
                    boundaryEdges.insert(make_pair(vStart, vEnd));
                }
                else {
                    boundaryEdges.erase(it);
                }
            }); // Now boundaryEdges contains only boundary edges

            Dynarray<Id> edgesPerVertex(verticesCount, 0);
            iterateEdges(triBegin, trianglesCount, [&edgesPerVertex, &boundaryEdges](const Id t, const Id i, const Id vStart, const Id vEnd) {
                if (vStart < vEnd && boundaryEdges.find(make_pair(vStart, vEnd)) == end(boundaryEdges)) ++edgesPerVertex[vStart];
            });

            for (const auto& edge : boundaryEdges) {
                ++edgesPerVertex[min(edge.first, edge.second)];
                // That's important to use min(..), because then writing twin we always search w/ min vertex
            }

            Dynarray<Id> edgesPerVertexAcc(verticesCount + 1);
            edgesPerVertexAcc[0] = 0;
            for (size_t v = 1; v < edgesPerVertexAcc.size(); ++v) {
                edgesPerVertexAcc[v] = edgesPerVertexAcc[v - 1] + edgesPerVertex[v - 1];
            }

            auto& availEdgesPerVertex = edgesPerVertex;
            Dynarray<Id> edgeToDcel(trianglesCount * 3, Traits<Id>::kNoIndex); // To let us lookup next location

            // I. Write down one twin for every edge
            iterateEdges(triBegin, trianglesCount, [&availEdgesPerVertex, &edgesPerVertexAcc, &mesh, &edgeToDcel](const Id t, const Id i, const Id vStart, const Id vEnd) {
                if (vStart > vEnd) {}
                else {
                    Id edgeIndex = edgesPerVertexAcc[vStart];
                    const Id edgesCount = edgesPerVertexAcc[vStart + 1] - edgesPerVertexAcc[vStart];
                    assert(availEdgesPerVertex[vStart] > 0);
                    const Id availEdgesCount = availEdgesPerVertex[vStart]--;
                    edgeIndex += edgesCount - availEdgesCount;
                    edgeIndex *= 2;
                    DebugPrintf("tri %d.%d - %d (%d, %d)\n", t, i, edgeIndex, vStart, vEnd);
                    assert(mesh.halfedges()[edgeIndex].getFace() == Traits<Id>::kNoIndex);
                    handleEdge(mesh, edgeIndex, t, vStart);
                    // It's possible that the edge is on the boundary,
                    // in this case we will not find twin for the half-edge.
                    // If we will, then we will just overwirite w/ proper values.
                    auto& halfedgeTwin = mesh.halfedges()[edgeIndex + 1];
                    halfedgeTwin.setFace(Traits<Id>::kOutside);
                    halfedgeTwin.setOrigin(vEnd);
                    edgeToDcel[t * 3 + i] = edgeIndex;
                }
            });
            assert(boundaryEdges.size() != 0 || find_if(cbegin(availEdgesPerVertex), cend(availEdgesPerVertex), [](Id i) { return i > 0; }) == cend(availEdgesPerVertex));
            DebugPrintf("\n");

            // II. Write down another twin for every edge
            iterateEdges(triBegin, trianglesCount, [&edgesPerVertexAcc, &boundaryEdges, &availEdgesPerVertex, &mesh, &edgeToDcel](const Id t, const Id i, const Id vStart, const Id vEnd) {
                if (vStart > vEnd) {
                    // Here i assume that max degree is small
                    bool found = false;
                    // Search either twin edge or empty slot
                    for (Id ei = edgesPerVertexAcc[vEnd]; ei < edgesPerVertexAcc[vEnd + 1]; ++ei) {
                        const Id edgeIndex = ei * 2 + 1;
                        if (mesh.halfedges()[edgeIndex - 1].getOrigin() == vEnd &&
                            mesh.halfedges()[edgeIndex].getOrigin() == vStart) 
                        {
                            DebugPrintf("tri %d.%d - %d (%d, %d)\n", t, i, edgeIndex, vStart, vEnd);
                            assert(mesh.halfedges()[edgeIndex].getFace() == Traits<Id>::kOutside);
                            handleEdge(mesh, edgeIndex, t, vStart);
                            found = true;
                        }
                        if (mesh.halfedges()[edgeIndex - 1].getOrigin() == Traits<Id>::kNoIndex &&
                            mesh.halfedges()[edgeIndex].getOrigin() == Traits<Id>::kNoIndex) {
                            DebugPrintf("tri %d.%d - %d (%d, %d)\n", t, i, edgeIndex, vStart, vEnd);
                            assert(mesh.halfedges()[edgeIndex].getFace() == Traits<Id>::kNoIndex);
                            handleEdge(mesh, edgeIndex, t, vStart);
                            auto& halfedgeTwin = mesh.halfedges()[edgeIndex - 1];
                            halfedgeTwin.setFace(Traits<Id>::kOutside);
                            halfedgeTwin.setOrigin(vEnd);
                            assert(boundaryEdges.find(make_pair(vStart, vEnd)) != end(boundaryEdges));
                            --availEdgesPerVertex[vEnd]; // Because vEnd is less than vStart
                            found = true;
                        }
                        if (found) {
                            edgeToDcel[t * 3 + i] = edgeIndex;
                            break;
                        }
                    }
                    assert(found);
                }
            });            
            assert(boundaryEdges.size() == 0 || find_if(cbegin(availEdgesPerVertex), cend(availEdgesPerVertex), [](Id i) { return i > 0; }) == cend(availEdgesPerVertex));

            // III. Update nexts
            iterateEdges(triBegin, trianglesCount, [&mesh, &edgeToDcel](const Id t, const Id i, const Id, const Id) {
                const uint32 currEdge = (t * 3 + i);
                const uint32 nextEdge = (t * 3 + (i + 1) % 3);
                mesh.halfedges()[edgeToDcel[currEdge]].setNext(edgeToDcel[nextEdge]);
            });            
            assert(count_if(cbegin(mesh.halfedges()), cend(mesh.halfedges()), [](const HalfEdge<Id>& he) { return he.getNext() == Traits<Id>::kNoIndex; }) == boundaryEdges.size());

            return mesh;
        }
    }
}
