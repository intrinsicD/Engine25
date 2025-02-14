//
// Created by alex on 2/15/25.
//

#ifndef TESTGRAPH_H
#define TESTGRAPH_H

#include "gtest/gtest.h"
#include "GraphUtils.h"

using namespace Bcg;

// Test fixture for Graph
class GraphTest : public ::testing::Test {
public:
    void Create() {
        A = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
        B = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
        C = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
        D = graph.add_vertex(Vector<Real, 3>(-1.0, 0.0, 0.0));
        E = graph.add_vertex(Vector<Real, 3>(0.0, -1.0, 0.0));

        AB = graph.add_edge(A, B);
        AC = graph.add_edge(A, C);
        BD = graph.add_edge(B, D);
        BE = graph.add_edge(B, E);
    }
    Vertex A, B, C, D, E;
    Halfedge AB, AC, BD, BE;
protected:
    Graph graph;
};

#endif //TESTGRAPH_H
