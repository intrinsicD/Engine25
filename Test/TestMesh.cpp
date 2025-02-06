//
// Created by alex on 13.01.25.
//

// TestGraph.cpp
#include "gtest/gtest.h"
#include "MeshUtils.h"

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
    EXPECT_EQ(mesh.edges.properties().size(), size_t(3));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edges.properties().size(), size_t(2));

    // implicit add
    eidx = mesh.edge_property<int>("e:idx2");
    EXPECT_EQ(mesh.edges.properties().size(), size_t(3));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edges.properties().size(), size_t(2));
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

// Helper: Create a tetrahedron mesh with vertices:
// v0 = (0,0,0), v1 = (1,0,0), v2 = (0,1,0), v3 = (0,0,1)
// Faces: (v0, v1, v2), (v0, v1, v3), (v0, v2, v3), (v1, v2, v3)
Mesh CreateTetrahedronMesh() {
    Mesh mesh;
    // Assuming that add_vertex takes a Vector<Real, 3> and returns a Vertex handle.
    Vertex v0 = mesh.add_vertex(Vector<Real, 3>(0.0, 0.0, 0.0));
    Vertex v1 = mesh.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    Vertex v2 = mesh.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    Vertex v3 = mesh.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
    // Assuming that add_face takes an initializer list of Vertex handles.
    mesh.add_face({v0, v1, v2});
    mesh.add_face({v0, v1, v3});
    mesh.add_face({v0, v2, v3});
    mesh.add_face({v1, v2, v3});
    return mesh;
}

// Helper: Create a single triangle mesh with vertices (0,0,0), (1,0,0), (0,1,0)
Mesh CreateSingleTriangleMesh() {
    Mesh mesh;
    Vertex v0 = mesh.add_vertex(Vector<Real, 3>(0.0, 0.0, 0.0));
    Vertex v1 = mesh.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    Vertex v2 = mesh.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    mesh.add_face({v0, v1, v2});
    return mesh;
}

//---------------------------------------------------------------
// Volume and Surface Area Tests
//---------------------------------------------------------------
TEST(MeshUtilsTest, VolumeTetrahedralDecomposition_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    // For the tetrahedron with vertices (0,0,0), (1,0,0), (0,1,0), (0,0,1)
    // the volume is 1/6.
    Real volume = VolumeTetrahedralDecomposition(mesh);
    EXPECT_NEAR(volume, 1.0 / 6.0, 1e-6);
}

TEST(MeshUtilsTest, VolumeDivergenceTheorem_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    Real volume = VolumeDivergenceTheorem(mesh);
    EXPECT_NEAR(volume, 1.0 / 6.0, 1e-6);
}

TEST(MeshUtilsTest, SurfaceArea_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    Real surfaceArea = SurfaceArea(mesh);
    // Expected area:
    // Three faces are right triangles with area 0.5 each, and the fourth face has area 0.5 * sqrt(3).
    Real expectedArea = 1.5 + 0.5 * std::sqrt(3.0);
    EXPECT_NEAR(surfaceArea, expectedArea, 1e-6);
}

//---------------------------------------------------------------
// Face Methods Tests (using a single triangle)
//---------------------------------------------------------------
TEST(MeshUtilsTest, FaceProperties_SingleTriangle) {
    Mesh mesh = CreateSingleTriangleMesh();
    // Assume that mesh.faces() returns a container of faces.
    const Face &f = *mesh.faces.begin();

    Real area = FaceArea(mesh, f);
    EXPECT_NEAR(area, 0.5, 1e-6);

    Vector<Real, 3> center = FaceCenter(mesh, f);
    Vector<Real, 3> expectedCenter(1.0 / 3.0, 1.0 / 3.0, 0.0);
    EXPECT_NEAR(center[0], expectedCenter[0], 1e-6);
    EXPECT_NEAR(center[1], expectedCenter[1], 1e-6);
    EXPECT_NEAR(center[2], expectedCenter[2], 1e-6);

    Vector<Real, 3> areaVector = FaceAreaVector(mesh, f);
    EXPECT_NEAR(areaVector.norm(), 0.5, 1e-6);

    Vector<Real, 3> normal = FaceNormal(mesh, f);
    // For this triangle in the XY plane, the normal should be (0,0,±1).
    EXPECT_NEAR(std::abs(normal[2]), 1.0, 1e-6);

    // Test FaceGradient:
    // Define a simple scalar field u = x coordinate.
    VertexProperty<Real> scalarfield(mesh.vertex_property<Real>("v:scalarfield"));
    for (const Vertex &v: mesh.vertices) {
        scalarfield[v] = mesh.positions[v][0];
    }
    Vector<Real, 3> grad = FaceGradient(mesh, f, scalarfield);
    // For the triangle with vertices (0,0,0), (1,0,0), (0,1,0), u = x, the gradient is (1, 0, 0).
    EXPECT_NEAR(grad[0], 1.0, 1e-6);
    EXPECT_NEAR(grad[1], 0.0, 1e-6);
    EXPECT_NEAR(grad[2], 0.0, 1e-6);
}

//---------------------------------------------------------------
// Vertex Methods Tests
//---------------------------------------------------------------
TEST(MeshUtilsTest, VertexAreasSum_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    Real sumVoronoi = 0.0;
    for (const Vertex &v: mesh.vertices) {
        sumVoronoi += VertexVoronoiArea(mesh, v);
    }
    Real surfaceArea = SurfaceArea(mesh);
    // For a proper mixed Voronoi area, the sum should equal the surface area.
    EXPECT_NEAR(sumVoronoi, surfaceArea, 1e-6);

    Real sumBarycentric = 0.0;
    for (const Vertex &v: mesh.vertices) {
        sumBarycentric += VertexBarycentricArea(mesh, v);
    }
    EXPECT_NEAR(sumBarycentric, surfaceArea, 1e-6);
}

TEST(MeshUtilsTest, VertexNormal_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    for (const Vertex &v: mesh.vertices) {
        Vector<Real, 3> vn = VertexNormal(mesh, v);
        EXPECT_NEAR(vn.norm(), 1.0, 1e-6);
    }
}

TEST(MeshUtilsTest, VertexStarCenterAndGradient_SingleTriangle) {
    Mesh mesh = CreateSingleTriangleMesh();
    // For a single triangle, each vertex's star center should be a weighted average
    // of the adjacent vertices. We simply test that it is finite.
    for (const Vertex &v: mesh.vertices) {
        Vector<Real, 3> starCenter = VertexStarCenter(mesh, v);
        EXPECT_TRUE(std::isfinite(starCenter[0]));
        EXPECT_TRUE(std::isfinite(starCenter[1]));
        EXPECT_TRUE(std::isfinite(starCenter[2]));
    }

    // For the star gradient, use the scalar field u = x.
    VertexProperty<Real> scalarfield = mesh.vertex_property<Real>("v:scalarfield");
    for (const Vertex &v: mesh.vertices) {
        scalarfield[v] = mesh.positions[v][0];
    }
    for (const Vertex &v: mesh.vertices) {
        Vector<Real, 3> starGrad = VertexStarGradient(mesh, v, scalarfield);
        // For a linear function u = x, the gradient should be parallel to (1,0,0).
        if (starGrad.norm() > 1e-6) {
            Vector<Real, 3> normalized = starGrad / starGrad.norm();
            EXPECT_NEAR(normalized[0], 1.0, 1e-6);
            EXPECT_NEAR(normalized[1], 0.0, 1e-6);
            EXPECT_NEAR(normalized[2], 0.0, 1e-6);
        }
    }
}

//---------------------------------------------------------------
// Dual Mesh Tests
//---------------------------------------------------------------
TEST(MeshUtilsTest, Dual_Tetrahedron) {
    Mesh mesh = CreateTetrahedronMesh();
    Mesh dualMesh = Dual(mesh);
    // For a closed mesh, the dual should have:
    //   - number of vertices equal to the number of faces of the original,
    //   - number of faces equal to the number of vertices of the original.
    EXPECT_EQ(dualMesh.vertices.size(), mesh.faces.size());
    EXPECT_EQ(dualMesh.faces.size(), mesh.vertices.size());
}
