//
// Created by alex on 2/3/25.
//

#ifndef GRAPHUTILS_H
#define GRAPHUTILS_H

#include "Graph.h"

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph);

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return pos[v1] - pos[v0];
    }

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_edge(h), pos);
    }

    template<typename T, int N>
    Real Length(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(v0, v1, pos).norm();
    }

    template<typename T, int N>
    Real Length(const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(e, pos).norm();
    }

    template<typename T, int N>
    Real Length(const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(h, pos).norm();
    }

    template<typename T, int N>
    Vector<T, N> Center(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return (pos[v0] + pos[v1]) / 2;
    }

    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_edge(h), pos);
    }
}

#endif //GRAPHUTILS_H
