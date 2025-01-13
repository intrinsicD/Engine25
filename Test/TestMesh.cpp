//
// Created by alex on 13.01.25.
//

// TestGraph.cpp
#include "gtest/gtest.h"
#include "Mesh.h"

using namespace Bcg;

// Test fixture for Graph
class MeshTest : public ::testing::Test {
protected:
    Mesh mesh;
    Vertex v0, v1, v2, v3;
    Face f0, f1;

    void add_triangle() {
        v0 = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
        v1 = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
        v2 = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }

    void add_triangles() {
        v0 = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
        v1 = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
        v2 = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));
        v3 = mesh.add_vertex(Vector<Real, 3>(1, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
        f1 = mesh.add_triangle(v1, v3, v2);
    }

    void add_quad() {
        v0 = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
        v1 = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
        v2 = mesh.add_vertex(Vector<Real, 3>(1, 1, 0));
        v3 = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));
        f0 = mesh.add_quad(v0, v1, v2, v3);
    }

    void vertex_onering() {
        auto v0 = mesh.add_vertex(Vector<Real, 3>(0.4499998093, 0.5196152329, 0.0000000000));
        auto v1 = mesh.add_vertex(Vector<Real, 3>(0.2999998033, 0.5196152329, 0.0000000000));
        auto v2 = mesh.add_vertex(Vector<Real, 3>(0.5249998569, 0.3897114396, 0.0000000000));
        auto v3 = mesh.add_vertex(Vector<Real, 3>(0.3749998510, 0.3897114396, 0.0000000000));
        auto v4 = mesh.add_vertex(Vector<Real, 3>(0.2249998450, 0.3897114396, 0.0000000000));
        auto v5 = mesh.add_vertex(Vector<Real, 3>(0.4499999285, 0.2598076165, 0.0000000000));
        auto v6 = mesh.add_vertex(Vector<Real, 3>(0.2999999225, 0.2598076165, 0.0000000000));

        mesh.add_triangle(v3, v0, v1);
        mesh.add_triangle(v3, v2, v0);
        mesh.add_triangle(v4, v3, v1);
        mesh.add_triangle(v5, v2, v3);
        mesh.add_triangle(v6, v5, v3);
        mesh.add_triangle(v6, v3, v4);
    }

    void edge_onering() {
        auto v0 = mesh.add_vertex(Vector<Real, 3>(0.5999997854, 0.5196152329, 0.0000000000));
        auto v1 = mesh.add_vertex(Vector<Real, 3>(0.4499998093, 0.5196152329, 0.0000000000));
        auto v2 = mesh.add_vertex(Vector<Real, 3>(0.2999998033, 0.5196152329, 0.0000000000));
        auto v3 = mesh.add_vertex(Vector<Real, 3>(0.6749998331, 0.3897114396, 0.0000000000));
        auto v4 = mesh.add_vertex(Vector<Real, 3>(0.5249998569, 0.3897114396, 0.0000000000));
        auto v5 = mesh.add_vertex(Vector<Real, 3>(0.3749998510, 0.3897114396, 0.0000000000));
        auto v6 = mesh.add_vertex(Vector<Real, 3>(0.2249998450, 0.3897114396, 0.0000000000));
        auto v7 = mesh.add_vertex(Vector<Real, 3>(0.5999999046, 0.2598076165, 0.0000000000));
        auto v8 = mesh.add_vertex(Vector<Real, 3>(0.4499999285, 0.2598076165, 0.0000000000));
        auto v9 = mesh.add_vertex(Vector<Real, 3>(0.2999999225, 0.2598076165, 0.0000000000));

        mesh.add_triangle(v4, v0, v1);
        mesh.add_triangle(v4, v3, v0);
        mesh.add_triangle(v5, v1, v2);
        mesh.add_triangle(v5, v4, v1);
        mesh.add_triangle(v6, v5, v2);
        mesh.add_triangle(v7, v3, v4);
        mesh.add_triangle(v8, v7, v4);
        mesh.add_triangle(v8, v4, v5);
        mesh.add_triangle(v9, v8, v5);
        mesh.add_triangle(v9, v5, v6);
    }
};

TEST_F(MeshTest, DefaultConstructor) {
    EXPECT_TRUE(mesh.is_empty());
    EXPECT_FALSE(mesh.has_garbage());
}

// Unit Test Examples
TEST_F(MeshTest, AddRemoveVertexTest) {
    auto v = mesh.add_vertex({1.0, 2.0, 3.0});
    ASSERT_TRUE(v.is_valid());
    EXPECT_EQ(mesh.n_vertices(), 1);
    EXPECT_EQ(mesh.positions[v], (Vector<Real, 3>(1.0, 2.0, 3.0)));
    mesh.delete_vertex(v);
    EXPECT_EQ(mesh.n_vertices(), 0);
    EXPECT_TRUE(mesh.has_garbage());
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), 0);
    EXPECT_FALSE(mesh.has_garbage());
}

TEST_F(MeshTest, AddTriangleTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    auto f = mesh.add_triangle(v0, v1, v2);
    ASSERT_TRUE(f.is_valid());

    EXPECT_EQ(mesh.n_faces(), 1);
    EXPECT_EQ(mesh.n_edges(), 3);
    EXPECT_EQ(mesh.n_vertices(), 3);
}

TEST_F(MeshTest, IsTriangleMeshTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    mesh.add_triangle(v0, v1, v2);
    ASSERT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(MeshTest, FindEdgeTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    mesh.add_triangle(v0, v1, v2);
    auto edge01 = mesh.find_edge(v0, v1);
    auto edge12 = mesh.find_edge(v1, v2);
    auto edge20 = mesh.find_edge(v2, v0);

    ASSERT_TRUE(edge01.is_valid());
    ASSERT_TRUE(edge12.is_valid());
    ASSERT_TRUE(edge20.is_valid());
}

TEST_F(MeshTest, IsFlipOkTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});
    auto v3 = mesh.add_vertex({1.0, 1.0, 0.0});

    auto f = mesh.add_quad(v0, v1, v2, v3);
    mesh.triangulate(f);

    ASSERT_TRUE(mesh.is_flip_ok(mesh.get_edge(mesh.get_halfedge(f))));
}


TEST_F(MeshTest, emptyMesh) {
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(MeshTest, insert_remove_single_vertex) {
    auto v = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
    EXPECT_EQ(mesh.n_vertices(), size_t(1));
    mesh.delete_vertex(v);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
}

TEST_F(MeshTest, insert_remove_single_triangle) {
    add_triangle();
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_edges(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(MeshTest, insert_remove_single_quad) {
    add_quad();
    EXPECT_EQ(mesh.n_vertices(), size_t(4));
    EXPECT_EQ(mesh.n_edges(), size_t(4));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(MeshTest, insert_remove_single_polygonal_face) {
    std::vector<Vertex> vertices(4);
    vertices[0] = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
    vertices[1] = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
    vertices[2] = mesh.add_vertex(Vector<Real, 3>(1, 1, 0));
    vertices[3] = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));

    auto f = mesh.add_face(vertices);
    EXPECT_EQ(mesh.n_vertices(), size_t(4));
    EXPECT_EQ(mesh.n_edges(), size_t(4));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(MeshTest, delete_center_vertex) {
    vertex_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(7));
    EXPECT_EQ(mesh.n_faces(), size_t(6));
    Vertex v(3); // the central vertex
    mesh.delete_vertex(v);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(MeshTest, delete_center_edge) {
    edge_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));
    // the two vertices of the center edge
    v0 = Vertex(4);
    v1 = Vertex(5);

    auto e = mesh.find_edge(v0, v1);
    mesh.delete_edge(e);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(8));
}

TEST_F(MeshTest, copy) {
    add_triangle();
    Mesh m2 = mesh;
    EXPECT_EQ(m2.n_vertices(), size_t(3));
    EXPECT_EQ(m2.n_edges(), size_t(3));
    EXPECT_EQ(m2.n_faces(), size_t(1));
}

TEST_F(MeshTest, assignment) {
    add_triangle();
    Mesh m2;
    m2.assign(mesh);
    EXPECT_EQ(m2.n_vertices(), size_t(3));
    EXPECT_EQ(m2.n_edges(), size_t(3));
    EXPECT_EQ(m2.n_faces(), size_t(1));
}

TEST_F(MeshTest, vertex_properties) {
    add_triangle();

    auto osize = mesh.vertices.properties().size();

    // explicit add
    auto vidx = mesh.add_vertex_property<int>("v:idx");
    vidx[v0] = 0;
    EXPECT_EQ(mesh.vertices.properties().size(), osize + 1);
    mesh.remove_vertex_property(vidx);
    EXPECT_EQ(mesh.vertices.properties().size(), osize);

    // implicit add
    vidx = mesh.vertex_property<int>("v:idx2");
    EXPECT_EQ(mesh.vertices.properties().size(), osize + 1);
    mesh.remove_vertex_property(vidx);
    EXPECT_EQ(mesh.vertices.properties().size(), osize);
}

TEST_F(MeshTest, halfedge_properties) {
    add_triangle();
    // explicit add
    auto hidx = mesh.add_halfedge_property<int>("h:idx");
    auto h = mesh.get_halfedge(v0);
    hidx[h] = 0;
    EXPECT_EQ(mesh.halfedges.properties().size(), size_t(3));
    mesh.remove_halfedge_property(hidx);
    EXPECT_EQ(mesh.halfedges.properties().size(), size_t(2));

    // implicit add
    hidx = mesh.halfedge_property<int>("h:idx2");
    EXPECT_EQ(mesh.halfedges.properties().size(), size_t(3));
    mesh.remove_halfedge_property(hidx);
    EXPECT_EQ(mesh.halfedges.properties().size(), size_t(2));
}

TEST_F(MeshTest, edge_properties) {
    add_triangle();
    // explicit add
    auto eidx = mesh.add_edge_property<int>("e:idx");
    auto e = mesh.get_edge(mesh.get_halfedge(v0));
    eidx[e] = 0;
    EXPECT_EQ(mesh.edges.properties().size(), size_t(2));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edges.properties().size(), size_t(1));

    // implicit add
    eidx = mesh.edge_property<int>("e:idx2");
    EXPECT_EQ(mesh.edges.properties().size(), size_t(2));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edges.properties().size(), size_t(1));
}

TEST_F(MeshTest, face_properties) {
    add_triangle();
    // explicit add
    auto fidx = mesh.add_face_property<int>("f:idx");
    fidx[f0] = 0;
    EXPECT_EQ(mesh.faces.properties().size(), size_t(3));
    mesh.remove_face_property(fidx);
    EXPECT_EQ(mesh.faces.properties().size(), size_t(2));

    // implicit add
    fidx = mesh.face_property<int>("f:idx2");
    EXPECT_EQ(mesh.faces.properties().size(), size_t(3));
    mesh.remove_face_property(fidx);
    EXPECT_EQ(mesh.faces.properties().size(), size_t(2));
}

TEST_F(MeshTest, vertex_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (auto v: mesh.vertices) {
        sumIdx += v.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(MeshTest, edge_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (auto e: mesh.edges) {
        sumIdx += e.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(MeshTest, halfedge_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (auto h: mesh.halfedges) {
        sumIdx += h.idx();
    }
    EXPECT_EQ(sumIdx, size_t(15));
}

TEST_F(MeshTest, face_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (auto f: mesh.faces) {
        sumIdx += f.idx();
    }
    EXPECT_EQ(sumIdx, size_t(0));
}

TEST_F(MeshTest, is_triangle_mesh) {
    add_triangle();
    EXPECT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(MeshTest, is_quad_mesh) {
    add_quad();
    EXPECT_TRUE(mesh.is_quad_mesh());
}

TEST_F(MeshTest, poly_mesh) {
    std::vector<Vertex> vertices(5);
    vertices[0] = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
    vertices[1] = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
    vertices[2] = mesh.add_vertex(Vector<Real, 3>(1, 1, 0));
    vertices[3] = mesh.add_vertex(Vector<Real, 3>(0.5, 1, 0));
    vertices[4] = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));
    mesh.add_face(vertices);
    EXPECT_FALSE(mesh.is_triangle_mesh() || mesh.is_quad_mesh());
}

TEST_F(MeshTest, vertex_valence) {
    add_triangle();
    auto val = mesh.get_valence(*mesh.vertices.begin());
    EXPECT_EQ(val, 2u);
}

TEST_F(MeshTest, face_valence) {
    add_triangle();
    auto val = mesh.get_valence(*mesh.faces.begin());
    EXPECT_EQ(val, 3u);
}

TEST_F(MeshTest, collapse) {
    add_triangles();
    EXPECT_EQ(mesh.n_faces(), size_t(2));
    auto h0 = mesh.find_halfedge(v3, v2);
    if (mesh.is_collapse_ok(h0))
        mesh.collapse(h0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(MeshTest, edge_removal_ok) {
    add_triangles();
    Edge e(1); // diagonal of triangulated quad
    EXPECT_TRUE(mesh.is_removal_ok(e));
}

TEST_F(MeshTest, edge_removal_not_ok) {
    add_triangle();
    Edge e(0); // boundary edge
    EXPECT_FALSE(mesh.is_removal_ok(e));
}

TEST_F(MeshTest, remove_edge) {
    add_triangles();
    Edge e(1); // diagonal of triangulated quad
    mesh.remove_edge(e);
    EXPECT_TRUE(mesh.is_quad_mesh());
}

TEST_F(MeshTest, face_split) {
    add_quad();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    Vector<Real, 3> p(0.5, 0.5, 0);
    mesh.split(f0, p);
    EXPECT_EQ(mesh.n_faces(), size_t(4));
}

TEST_F(MeshTest, edge_split) {
    add_triangle();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    auto e = mesh.find_edge(v0, v1);
    Vector<Real, 3> p0 = mesh.positions[v0];
    Vector<Real, 3> p1 = mesh.positions[v1];
    Vector<Real, 3> p = (p0 + p1) * 0.5;
    mesh.split(e, p);
    EXPECT_EQ(mesh.n_faces(), size_t(2));
}

TEST_F(MeshTest, edge_flip) {
    edge_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));

    // the two vertices of the center edge
    v0 = Vertex(4);
    v1 = Vertex(5);
    auto e = mesh.find_edge(v0, v1);
    if (mesh.is_flip_ok(e)) {
        mesh.flip(e);
    }
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));
}

TEST_F(MeshTest, is_manifold) {
    vertex_onering();
    for (auto v: mesh.vertices) {
        EXPECT_TRUE(mesh.is_manifold(v));
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Iterator Tests
//----------------------------------------------------------------------------------------------------------------------

using It = Vertices::VertexIterator;
TEST_F(MeshTest, default_constructible) {
    It a;
    EXPECT_FALSE((*a).is_valid());
}

TEST_F(MeshTest, copy_constructible) {
    It a;
    It b(a);
    EXPECT_FALSE((*b).is_valid());
}

TEST_F(MeshTest, assignable) {
    It a;
    auto b = a;
    EXPECT_FALSE((*b).is_valid());
}

TEST_F(MeshTest, insert) {
    auto mesh = Mesh{};
    auto vertices = std::vector<Vertex>{};
    mesh.add_vertex(Vector<Real, 3>::Zero());
    vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
    EXPECT_EQ(vertices.size(), 1u);
}

TEST_F(MeshTest, std_distance) {
    vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    auto d = std::distance(vv.begin(), vv.end());
    EXPECT_EQ(d, 6);
}

TEST_F(MeshTest, post_increment) {
    vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    EXPECT_EQ((*vv++).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 6u);
}

TEST_F(MeshTest, post_decrement) {
    vertex_onering();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    EXPECT_EQ((*vv--).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 1u);
}
