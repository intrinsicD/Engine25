//
// Created by alex on 2/10/25.
//

#include "MeshShapes.h"
#include "MeshUtils.h"
#include "MeshSubdivision.h"
#include "MeshFeatures.h"

namespace Bcg {
    void ProjectToUnitSphere(Mesh &mesh) {
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");
        Eigen::Map<Eigen::Matrix<Real, Eigen::Dynamic, 3, Eigen::RowMajor> > P(
            positions[Vertex(0)].data(), positions.size(), 3);
        P.rowwise().normalize();
    }


    Mesh VertexOneRing() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");
        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499998093, 0.5196152329, 0.0000000000));
        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999998033, 0.5196152329, 0.0000000000));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5249998569, 0.3897114396, 0.0000000000));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.3749998510, 0.3897114396, 0.0000000000));
        auto v4 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2249998450, 0.3897114396, 0.0000000000));
        auto v5 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499999285, 0.2598076165, 0.0000000000));
        auto v6 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999999225, 0.2598076165, 0.0000000000));

        mesh.add_triangle(v3, v0, v1);
        mesh.add_triangle(v3, v2, v0);
        mesh.add_triangle(v4, v3, v1);
        mesh.add_triangle(v5, v2, v3);
        mesh.add_triangle(v6, v5, v3);
        mesh.add_triangle(v6, v3, v4);

        return mesh;
    }

    Mesh EdgeOneRing() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");
        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5999997854, 0.5196152329, 0.0000000000));
        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499998093, 0.5196152329, 0.0000000000));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999998033, 0.5196152329, 0.0000000000));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.6749998331, 0.3897114396, 0.0000000000));
        auto v4 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5249998569, 0.3897114396, 0.0000000000));
        auto v5 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.3749998510, 0.3897114396, 0.0000000000));
        auto v6 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2249998450, 0.3897114396, 0.0000000000));
        auto v7 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5999999046, 0.2598076165, 0.0000000000));
        auto v8 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499999285, 0.2598076165, 0.0000000000));
        auto v9 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999999225, 0.2598076165, 0.0000000000));

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

        return mesh;
    }

    Mesh SubdividedIcosahedron() {
        // use ref for brevity
        auto mesh = Icosahedron();

        // select all edges as features
        DetectFeatures(mesh, 25);

        // feature-preserving subdivision
        Subdivision::Loop(mesh);
        Subdivision::Loop(mesh);
        Subdivision::Loop(mesh);
        return mesh;
    }

    Mesh LShape() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");
        std::vector<Vertex> vertices;

        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 0.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5, 0.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(1.0, 0.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(1.0, 0.5, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5, 0.5, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5, 1.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5, 1.5, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5, 2.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 2.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 1.5, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 1.0, 0.0)));
        vertices.push_back(add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 0.5, 0.0)));

        mesh.add_face(vertices);

        return mesh;
    }

    Mesh OpenCone() {
        auto mesh = Cone(8, 1, 1.5);
        for (auto f: mesh.faces)
            if (mesh.get_valence(f) > 3) {
                mesh.delete_face(f);
                mesh.garbage_collection();
                break;
            }
        return mesh;
    }

    Mesh TextureSeamsMesh() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5999997854, 0.5196152329, 0.0000000000));
        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499998093, 0.5196152329, -0.001000000));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999998033, 0.5196152329, 0.0000000000));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.6749998331, 0.3897114396, -0.001000000));
        auto v4 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5249998569, 0.3897114396, 0.0000000000));
        auto v5 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.3749998510, 0.3897114396, 0.0000000000));
        auto v6 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2249998450, 0.3897114396, 0.0000000000));
        auto v7 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5999999046, 0.2598076165, 0.0000000000));
        auto v8 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.4499999285, 0.2598076165, 0.0000000000));
        auto v9 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.2999999225, 0.2598076165, 0.0000000000));
        auto v10 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.749999285, 0.2598076165, 0.0000000000));
        auto v11 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.8249998331, 0.3897114396, 0.0000000000));
        auto v12 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.749999285, 0.5196152329, 0.0000000000));
        auto v13 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.6749998331, 0.6496152329, 0.0000000000));
        auto v14 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.5249998569, 0.6496152329, 0.0000000000));
        auto v15 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.3749998510, 0.6496152329, 0.0000000000));

        mesh.add_triangle(v4, v0, v1);
        mesh.add_triangle(v4, v3, v0);
        mesh.add_triangle(v15, v4, v1);
        mesh.add_triangle(v2, v5, v4);
        mesh.add_triangle(v6, v5, v2);
        mesh.add_triangle(v7, v11, v4);
        mesh.add_triangle(v8, v7, v4);
        mesh.add_triangle(v8, v4, v5);
        mesh.add_triangle(v9, v8, v5);
        mesh.add_triangle(v9, v5, v6);
        mesh.add_triangle(v7, v10, v11);
        mesh.add_triangle(v4, v11, v3);
        mesh.add_triangle(v3, v11, v12);
        mesh.add_triangle(v3, v12, v0);
        mesh.add_triangle(v0, v12, v13);
        mesh.add_triangle(v0, v13, v14);
        mesh.add_triangle(v0, v14, v1);
        mesh.add_triangle(v1, v14, v15);
        mesh.add_triangle(v2, v4, v15);

        // add test texcoords
        auto texcoords = mesh.halfedge_property<Vector<Real, 2> >("h:tex");

        for (auto v: mesh.vertices) {
            const Vector<Real, 3> &p = positions[v];
            for (auto h: mesh.get_halfedges(v)) {
                if (mesh.is_boundary(mesh.get_opposite(h))) {
                    continue;
                }
                texcoords[mesh.get_opposite(h)] = Vector<Real, 2>(p[0], p[1]);
            }
        }

        // change texcoords to create a texture seam
        std::vector<Face> faces = {
            Face(0), Face(1), Face(12), Face(13),
            Face(14), Face(15), Face(16), Face(17)
        };
        for (auto f: faces) {
            for (auto h: mesh.get_halfedges(f)) {
                texcoords[h] += Vector<Real, 2>(0.1, 0.1);
            }
        }

        return mesh;
    }


    Mesh Tetrahedron() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        float a = 1.0f / 3.0f;
        float b = sqrt(8.0f / 9.0f);
        float c = sqrt(2.0f / 9.0f);
        float d = sqrt(2.0f / 3.0f);

        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0, 0, 1));
        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-c, d, -a));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-c, -d, -a));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(b, 0, -a));

        mesh.add_triangle(v0, v1, v2);
        mesh.add_triangle(v0, v2, v3);
        mesh.add_triangle(v0, v3, v1);
        mesh.add_triangle(v3, v2, v1);

        return mesh;
    }

    Mesh Hexahedron() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        float a = 1.0f / sqrt(3.0f);
        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, -a, -a));
        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, -a, -a));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, a, -a));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, a, -a));
        auto v4 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, -a, a));
        auto v5 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, -a, a));
        auto v6 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, a, a));
        auto v7 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, a, a));

        mesh.add_quad(v3, v2, v1, v0);
        mesh.add_quad(v2, v6, v5, v1);
        mesh.add_quad(v5, v6, v7, v4);
        mesh.add_quad(v0, v4, v7, v3);
        mesh.add_quad(v3, v7, v6, v2);
        mesh.add_quad(v1, v5, v4, v0);

        return mesh;
    }

    Mesh Octahedron() {
        auto mesh = Dual(Hexahedron());
        ProjectToUnitSphere(mesh);
        return mesh;
    }

    Mesh dodecahedron() {
        auto mesh = Dual(Icosahedron());
        ProjectToUnitSphere(mesh);
        return mesh;
    }

    Mesh Icosahedron() {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
        float a = 1.0f;
        float b = 1.0f / phi;

        auto v1 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0, b, -a));
        auto v2 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(b, a, 0));
        auto v3 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-b, a, 0));
        auto v4 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0, b, a));
        auto v5 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0, -b, a));
        auto v6 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, 0, b));
        auto v7 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0, -b, -a));
        auto v8 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, 0, -b));
        auto v9 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(a, 0, b));
        auto v10 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-a, 0, -b));
        auto v11 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(b, -a, 0));
        auto v12 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(-b, -a, 0));

        ProjectToUnitSphere(mesh);

        mesh.add_triangle(v3, v2, v1);
        mesh.add_triangle(v2, v3, v4);
        mesh.add_triangle(v6, v5, v4);
        mesh.add_triangle(v5, v9, v4);
        mesh.add_triangle(v8, v7, v1);
        mesh.add_triangle(v7, v10, v1);
        mesh.add_triangle(v12, v11, v5);
        mesh.add_triangle(v11, v12, v7);
        mesh.add_triangle(v10, v6, v3);
        mesh.add_triangle(v6, v10, v12);
        mesh.add_triangle(v9, v8, v2);
        mesh.add_triangle(v8, v9, v11);
        mesh.add_triangle(v3, v6, v4);
        mesh.add_triangle(v9, v2, v4);
        mesh.add_triangle(v10, v3, v1);
        mesh.add_triangle(v2, v8, v1);
        mesh.add_triangle(v12, v10, v7);
        mesh.add_triangle(v8, v11, v7);
        mesh.add_triangle(v6, v12, v5);
        mesh.add_triangle(v11, v9, v5);

        return mesh;
    }

    Mesh Icosphere(size_t n_subdivisions) {
        auto mesh = Icosahedron();
        for (size_t i = 0; i < n_subdivisions; i++) {
            Subdivision::Loop(mesh);
            ProjectToUnitSphere(mesh);
        }
        return mesh;
    }

    Mesh QuadSphere(size_t n_subdivisions) {
        auto mesh = Hexahedron();

        for (size_t i = 0; i < n_subdivisions; i++) {
            Subdivision::CatmullClark(mesh);
            ProjectToUnitSphere(mesh);
        }
        return mesh;
    }

    Mesh UVSphere(const Vector<Real, 3> &center, Real radius, size_t n_slices,
                  size_t n_stacks) {
        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // add top vertex
        const auto top = Vector<Real, 3>(center[0], center[1] + radius, center[2]);
        const auto v0 = add_vertex(mesh.vertices, positions, top);

        // generate vertices per stack / slice
        for (size_t i = 0; i < n_stacks - 1; i++) {
            const auto phi = std::numbers::pi * double(i + 1) / double(n_stacks);
            for (size_t j = 0; j < n_slices; ++j) {
                const auto theta =
                        2.0 * std::numbers::pi * double(j) / double(n_slices);
                const auto x = center[0] + radius * std::sin(phi) * std::cos(theta);
                const auto y = center[1] + radius * std::cos(phi);
                const auto z = center[2] + radius * std::sin(phi) * std::sin(theta);
                add_vertex(mesh.vertices, positions, Vector<Real, 3>(x, y, z));
            }
        }

        // add bottom vertex
        const auto bottom = Vector<Real, 3>(center[0], center[1] - radius, center[2]);
        const auto v1 = add_vertex(mesh.vertices, positions, bottom);

        // add top / bottom triangles
        for (size_t i = 0; i < n_slices; ++i) {
            const auto i0 = i + 1;
            const auto i1 = (i + 1) % n_slices + 1;
            mesh.add_triangle(v0, Vertex(i1), Vertex(i0));

            const auto i2 = i + n_slices * (n_stacks - 2) + 1;
            const auto i3 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
            mesh.add_triangle(v1, Vertex(i2), Vertex(i3));
        }

        // add quads per stack / slice
        for (size_t j = 0; j < n_stacks - 2; ++j) {
            const auto idx0 = j * n_slices + 1;
            const auto idx1 = (j + 1) * n_slices + 1;
            for (size_t i = 0; i < n_slices; ++i) {
                const auto i0 = idx0 + i;
                const auto i1 = idx0 + (i + 1) % n_slices;
                const auto i2 = idx1 + (i + 1) % n_slices;
                const auto i3 = idx1 + i;
                mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3));
            }
        }

        return mesh;
    }

    Mesh Plane(size_t resolution) {
        assert(resolution >= 1);

        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // generate vertices
        Vector<Real, 3> p(0, 0, 0);
        for (size_t i = 0; i < resolution + 1; i++) {
            for (size_t j = 0; j < resolution + 1; j++) {
                add_vertex(mesh.vertices, positions, p);
                p[1] += 1.0 / resolution;
            }
            p[1] = 0;
            p[0] += 1.0 / resolution;
        }

        // generate faces
        for (size_t i = 0; i < resolution; i++) {
            for (size_t j = 0; j < resolution; j++) {
                auto v0 = Vertex(j + i * (resolution + 1));
                auto v1 = Vertex(v0.idx() + resolution + 1);
                auto v2 = Vertex(v0.idx() + resolution + 2);
                auto v3 = Vertex(v0.idx() + 1);
                mesh.add_quad(v0, v1, v2, v3);
            }
        }

        return mesh;
    }

    Mesh Cone(size_t n_subdivisions, Real radius, Real height) {
        assert(n_subdivisions >= 3);

        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // add vertices subdividing a circle
        std::vector<Vertex> base_vertices;
        for (size_t i = 0; i < n_subdivisions; i++) {
            Real ratio = static_cast<Real>(i) / (n_subdivisions);
            Real r = ratio * (std::numbers::pi * 2.0);
            Real x = std::cos(r) * radius;
            Real y = std::sin(r) * radius;
            auto v = add_vertex(mesh.vertices, positions, Vector<Real, 3>(x, y, 0.0));
            base_vertices.push_back(v);
        }

        // add the tip of the cone
        auto v0 = add_vertex(mesh.vertices, positions, Vector<Real, 3>(0.0, 0.0, height));

        // generate triangular faces
        for (size_t i = 0; i < n_subdivisions; i++) {
            auto ii = (i + 1) % n_subdivisions;
            mesh.add_triangle(v0, Vertex(i), Vertex(ii));
        }

        // reverse order for consistent face orientation
        std::reverse(base_vertices.begin(), base_vertices.end());

        // add polygonal base face
        mesh.add_face(base_vertices);

        return mesh;
    }

    Mesh Cylinder(size_t n_subdivisions, Real radius, Real height) {
        assert(n_subdivisions >= 3);

        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // generate vertices
        std::vector<Vertex> bottom_vertices;
        std::vector<Vertex> top_vertices;
        for (size_t i = 0; i < n_subdivisions; i++) {
            Real ratio = static_cast<Real>(i) / (n_subdivisions);
            Real r = ratio * (std::numbers::pi * 2.0);
            Real x = std::cos(r) * radius;
            Real y = std::sin(r) * radius;
            Vertex v = add_vertex(mesh.vertices, positions, Vector<Real, 3>(x, y, 0.0));
            bottom_vertices.push_back(v);
            v = add_vertex(mesh.vertices, positions, Vector<Real, 3>(x, y, height));
            top_vertices.push_back(v);
        }

        // add faces around the cylinder
        for (size_t i = 0; i < n_subdivisions; i++) {
            auto ii = i * 2;
            auto jj = (ii + 2) % (n_subdivisions * 2);
            auto kk = (ii + 3) % (n_subdivisions * 2);
            auto ll = ii + 1;
            mesh.add_quad(Vertex(ii), Vertex(jj), Vertex(kk), Vertex(ll));
        }

        // add top polygon
        mesh.add_face(top_vertices);

        // reverse order for consistent face orientation
        std::reverse(bottom_vertices.begin(), bottom_vertices.end());

        // add bottom polygon
        mesh.add_face(bottom_vertices);

        return mesh;
    }

    Mesh Torus(size_t radial_resolution, size_t tubular_resolution,
               Real radius, Real thickness) {
        assert(radial_resolution >= 3);
        assert(tubular_resolution >= 3);

        Mesh mesh;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // generate vertices
        for (size_t i = 0; i < radial_resolution; i++) {
            for (size_t j = 0; j < tubular_resolution; j++) {
                Real u = static_cast<Real>(j) / tubular_resolution * std::numbers::pi * 2.0;
                Real v = static_cast<Real>(i) / radial_resolution * std::numbers::pi * 2.0;
                Real x = (radius + thickness * std::cos(v)) * std::cos(u);
                Real y = (radius + thickness * std::cos(v)) * std::sin(u);
                Real z = thickness * std::sin(v);
                add_vertex(mesh.vertices, positions, Vector<Real, 3>(x, y, z));
            }
        }

        // add quad faces
        for (size_t i = 0; i < radial_resolution; i++) {
            auto i_next = (i + 1) % radial_resolution;
            for (size_t j = 0; j < tubular_resolution; j++) {
                auto j_next = (j + 1) % tubular_resolution;
                auto i0 = i * tubular_resolution + j;
                auto i1 = i * tubular_resolution + j_next;
                auto i2 = i_next * tubular_resolution + j_next;
                auto i3 = i_next * tubular_resolution + j;
                mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3));
            }
        }

        return mesh;
    }
};
