//
// Created by alex on 13.01.25.
//

#include <MeshFeatures.h>
#include <MeshSubdivision.h>

#include "gtest/gtest.h"
#include "MeshShapes.h"
#include "MeshUtils.h"
#include "TriangleUtils.h"

using namespace Bcg;

using It = Vertices::VertexIterator;

// Note: These are partial tests only used for development. They are not meant
// to fully cover iterators and their functionality,

TEST(VertexIteratorTest, default_constructible) {
    It a;
    EXPECT_FALSE((*a).is_valid());
}

TEST(VertexIteratorTest, copy_constructible) {
    It a;
    It b(a);
    EXPECT_FALSE((*b).is_valid());
}

TEST(VertexIteratorTest, assignable) {
    It a;
    auto b = a;
    EXPECT_FALSE((*b).is_valid());
}

TEST(VertexIteratorTest, insert) {
    auto mesh = Mesh{};
    auto vertices = std::vector<Vertex>{};
    mesh.add_vertex(Vector<Real, 3>::Zero());
    vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
    EXPECT_EQ(vertices.size(), 1u);
}

TEST(VertexCirculatorTest, std_distance) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    auto d = std::distance(vv.begin(), vv.end());
    EXPECT_EQ(d, 6);
}

TEST(VertexCirculatorTest, post_increment) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    EXPECT_EQ((*vv++).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 6u);
}

TEST(VertexCirculatorTest, post_decrement) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    EXPECT_EQ((*vv--).idx(), 4u);
    EXPECT_EQ((*vv).idx(), 1u);
}


TEST(CirculatorTest, vertex_vertex_circulator) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vv = mesh.get_vertices(v);
    auto d = std::distance(vv.begin(), vv.end());
    EXPECT_EQ(d, 6);
}

TEST(CirculatorTest, vertex_halfedge_circulator) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vh = mesh.get_halfedges(v);
    auto d = std::distance(vh.begin(), vh.end());
    EXPECT_EQ(d, 6);
}

TEST(CirculatorTest, vertex_edge_circulator) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto ve = mesh.get_edges(v);
    auto d = std::distance(ve.begin(), ve.end());
    EXPECT_EQ(d, 6);
}

TEST(CirculatorTest, vertex_face_circulator) {
    auto mesh = VertexOneRing();
    auto v = Vertex(3); // center vertex
    auto vf = mesh.get_faces(v);
    auto d = std::distance(vf.begin(), vf.end());
    EXPECT_EQ(d, 6);
}

TEST(CirculatorTest, face_halfedge_circulator) {
    auto mesh = VertexOneRing();
    auto f = Face(0); // center vertex
    auto fh = mesh.get_halfedges(f);
    auto d = std::distance(fh.begin(), fh.end());
    EXPECT_EQ(d, 3);
}

TEST(CirculatorTest, face_vertex_circulator) {
    auto mesh = VertexOneRing();
    auto f = Face(0); // center vertex
    auto fv = mesh.get_vertices(f);
    auto d = std::distance(fv.begin(), fv.end());
    EXPECT_EQ(d, 3);
}

//----------------------------------------------------------------------------------------------------------------------
// Differential Geometry Tests
//----------------------------------------------------------------------------------------------------------------------

class DifferentialGeometryTest : public ::testing::Test {
public:
    Mesh mesh;
    static Mesh sphere;

    Vertex v0, v1, v2, v3;
    Face f0;

    Vertex central_vertex;

    void add_triangle() {
        v0 = mesh.add_vertex(Vector<Real, 3>(0, 0, 0));
        v1 = mesh.add_vertex(Vector<Real, 3>(1, 0, 0));
        v2 = mesh.add_vertex(Vector<Real, 3>(0, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }

    void one_ring() {
        mesh = VertexOneRing();
        central_vertex = Vertex(3); // the central vertex
        mesh.positions[central_vertex][2] = 0.1; // lift central vertex
    }
};

Mesh DifferentialGeometryTest::sphere = Icosphere(5);

TEST_F(DifferentialGeometryTest, area_points) {
    add_triangle();
    Real a = TriangleArea(mesh.positions[v0], mesh.positions[v1], mesh.positions[v2]);
    EXPECT_EQ(a, 0.5);
}

TEST_F(DifferentialGeometryTest, area_face) {
    add_triangle();
    Real a = FaceArea(mesh, f0);
    EXPECT_EQ(a, 0.5);
}

TEST_F(DifferentialGeometryTest, area_vertex) {
    one_ring();
    Real a = VertexBarycentricArea(mesh, central_vertex);
    EXPECT_FLOAT_EQ(a, 0.024590395);
}

TEST_F(DifferentialGeometryTest, laplace) {
    one_ring();
    auto lv = VertexLaplace(mesh, central_vertex);
    EXPECT_GT(lv.norm(), 0);
}

TEST_F(DifferentialGeometryTest, area_surface) {
    auto a = SurfaceArea(sphere);
    EXPECT_NEAR(a, 12.57, 1.0e-2);
    double sum_voronoi_mixed = 0;
    for (const auto &v: sphere.vertices) {
        sum_voronoi_mixed += VertexVoronoiMixedArea(sphere, v);
    }
    EXPECT_NEAR(sum_voronoi_mixed, 12.57, 1.0e-2);
}

TEST_F(DifferentialGeometryTest, volume) {
    auto vdt = VolumeDivergenceTheorem(sphere);
    EXPECT_NEAR(vdt, 4.18, 1.0e-2);

    auto vtd = VolumeTetrahedralDecomposition(sphere);
    EXPECT_NEAR(vtd, 4.18, 1.0e-2);
}

TEST_F(DifferentialGeometryTest, centroid) {
    auto center = Centroid(sphere);
    EXPECT_LT(center.norm(), 1e-5);
}

TEST_F(DifferentialGeometryTest, barycentric_coords) {
    add_triangle();
    Vector<Real, 3> bc_center = Vector<Real, 3>(1. / 3., 1. / 3., 1. / 3.);
    Vector<Real, 3> result = FaceFromBarycentricCoordinates(mesh, f0, bc_center);
    Vector<Real, 3> bc = FaceToBarycentricCoordinates(mesh, f0, result);
    for (int i = 0; i < 3; ++i) {
        EXPECT_NEAR(bc_center[i], bc[i], 1e-7);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Mesh Subdivision Tests
//----------------------------------------------------------------------------------------------------------------------


TEST(SubdivisionTest, loop_subdivision) {
    auto mesh = Icosahedron();
    Subdivision::Loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_features) {
    auto mesh = Icosahedron();
    DetectFeatures(mesh, 25);
    Subdivision::Loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(80));
}

TEST(SubdivisionTest, loop_with_boundary) {
    auto mesh = VertexOneRing();
    Subdivision::Loop(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_subdivision) {
    auto mesh = Hexahedron();
    Subdivision::CatmullClark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_features) {
    auto mesh = Hexahedron();
    DetectFeatures(mesh, 25);
    Subdivision::CatmullClark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, catmull_clark_with_boundary) {
    auto mesh = Plane(1);
    Subdivision::CatmullClark(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(4));
}

TEST(SubdivisionTest, quad_tri_on_quads) {
    auto mesh = Hexahedron();
    Subdivision::QuadTri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(24));
}

TEST(SubdivisionTest, quad_tri_on_triangles) {
    auto mesh = Tetrahedron();
    Subdivision::QuadTri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(16));
}

TEST(SubdivisionTest, quad_tri_on_mixed) {
    auto mesh = Cone(4); // pyramid
    Subdivision::QuadTri(mesh);
    EXPECT_EQ(mesh.n_faces(), size_t(20));
}
