//
// Created by alex on 18.12.24.
//

#ifndef ENGINE25_GRAPH_H
#define ENGINE25_GRAPH_H

#include "Properties.h"
#include "PropertiesHandle.h"

namespace Bcg {
    class Graph {
    public:
        Graph() : vertex_connectivity(vertices.Add<VertexConnectivity>("v:connectivity")),
                  halfedge_connectivity(halfedges.Add<HalfEdgeConnectivity>("h:connectivity")),
                  edge_direction(edges.Add<unsigned int>("e:direction", 0)) {}

        virtual ~Graph() = default;

        struct VertexConnectivity {
            HalfEdgeHandle halfedge;
        };

        struct HalfEdgeConnectivity {
            HalfEdgeHandle next;
            HalfEdgeHandle prev;
            VertexHandle vertex;
        };

        Property<VertexConnectivity> vertex_connectivity;
        Property<HalfEdgeConnectivity> halfedge_connectivity;
        Property<unsigned int> edge_direction; // -1, 0, 1  for v0 -> v1, no direction, v1 -> v0

        HalfEdgeHandle NewHalfEdge() {
            halfedges.PushBack();
            return halfedges.Size() - 1;
        }

        EdgeHandle NewEdge() {
            edges.PushBack();
            auto h0 = NewHalfEdge();
            auto h1 = NewHalfEdge();
            SetNext(h0, h1);
            SetPrev(h1, h0);
            return edges.Size() - 1;
        }

        HalfEdgeHandle FindHalfedge(VertexHandle start, VertexHandle end) const {
            auto h = GetHalfEdge(start);
            auto h0 = h;
            do {
                if (GetVertex(h) == end) {
                    return h;
                }
                h = Next(h);
            } while (h != h0);
            return -1;
        }

        EdgeHandle AddEdge(VertexHandle v0, VertexHandle v1) {
            // Check if the half-edge already exists.
            auto h = FindHalfedge(v0, v1);
            if (h != -1) {
                return GetEdge(h); // Return the existing edge if found.
            }

            // Create a new edge with two half-edges.
            auto e = NewEdge();
            auto h0 = GetHalfEdge(e, 0); // Half-edge from v0 to v1.
            auto h1 = GetHalfEdge(e, 1); // Half-edge from v1 to v0.

            // Assign vertices to the half-edges.
            SetVertex(h0, v0);
            SetVertex(h1, v1);

            // Get the current outgoing half-edges of v0 and v1.
            auto hv0 = GetHalfEdge(v0);
            auto hv1 = GetHalfEdge(v1);

            // Update the vertices' outgoing half-edges to the new ones.
            SetHalfEdge(v0, h0);
            SetHalfEdge(v1, h1);

            // Set Prev and Next for the new half-edges based on existing ones.

            // Handle v0's half-edge loop.
            if (hv0 != -1) {
                auto h0_next = Next(hv0);
                SetNext(h0, h0_next);
                SetPrev(h0_next, h0);
                SetNext(hv0, h0);
                SetPrev(h0, hv0);
            } else {
                // If no outgoing half-edge, create a self-loop.
                SetNext(h0, h1);
                SetPrev(h1, h0);
            }

            // Handle v1's half-edge loop.
            if (hv1 != -1) {
                auto h1_next = Next(hv1);
                SetNext(h1, h1_next);
                SetPrev(h1_next, h1);
                SetNext(hv1, h1);
                SetPrev(h1, hv1);
            } else {
                // If no outgoing half-edge, create a self-loop.
                SetNext(h1, h0);
                SetPrev(h0, h1);
            }

            return e; // Return the newly created edge.
        }

        void MarkDirection(EdgeHandle e, unsigned int i) {
            edge_direction[e] = i;
        }

        VertexHandle NewVertex() {
            vertices.PushBack();
            return vertices.Size() - 1;
        }

        void SetVertex(HalfEdgeHandle h, VertexHandle v) {
            halfedge_connectivity[h].vertex = v;
        }

        VertexHandle GetVertex(HalfEdgeHandle h) const {
            return halfedge_connectivity[h].vertex;
        }

        HalfEdgeHandle Next(HalfEdgeHandle h) const {
            return halfedge_connectivity[h].next;
        }

        HalfEdgeHandle Prev(HalfEdgeHandle h) const {
            return halfedge_connectivity[h].prev;
        }

        void SetNext(HalfEdgeHandle h, HalfEdgeHandle next) {
            halfedge_connectivity[h].next = next;
        }

        void SetPrev(HalfEdgeHandle h, HalfEdgeHandle prev) {
            halfedge_connectivity[h].prev = prev;
        }

        void SetHalfEdge(VertexHandle v, HalfEdgeHandle h) {
            vertex_connectivity[v].halfedge = h;
        }

        HalfEdgeHandle GetHalfEdge(VertexHandle v) const {
            return vertex_connectivity[v].halfedge;
        }

        HalfEdgeHandle GetHalfEdge(EdgeHandle e, unsigned int i) const {
            return e * 2 + i;
        }

        EdgeHandle GetEdge(HalfEdgeHandle h) const {
            return h / 2;
        }

    private:
        PropertyContainer vertices;
        PropertyContainer edges;
        PropertyContainer halfedges;
    };
}

#endif //ENGINE25_GRAPH_H
