//
// Created by alex on 2/10/25.
//

#include "MeshSubdivision.h"
#include "MeshUtils.h"

namespace Bcg::Subdivision {
    void CatmullClark(Mesh &mesh, BoundaryHandling boundary_handling) {
        auto points_ = mesh.vertex_property<Vector<Real, 3> >("v:position");
        auto vfeature_ = mesh.get_vertex_property<bool>("v:feature");
        auto efeature_ = mesh.get_edge_property<bool>("e:feature");

        // reserve memory
        size_t nv = mesh.n_vertices();
        size_t ne = mesh.n_edges();
        size_t nf = mesh.n_faces();
        mesh.reserve(nv + ne + nf, 2 * ne + 4 * nf, 4 * nf);

        // get properties
        auto vpoint = mesh.add_vertex_property<Vector<Real, 3> >("catmull:vpoint");
        auto epoint = mesh.add_edge_property<Vector<Real, 3> >("catmull:epoint");
        auto fpoint = mesh.add_face_property<Vector<Real, 3> >("catmull:fpoint");

        // compute face vertices
        for (auto f: mesh.faces) {
            fpoint[f] = FaceCenter(mesh, f);
        }

        // compute edge vertices
        for (auto e: mesh.edges) {
            // boundary or feature edge?
            if (mesh.is_boundary(e) || (efeature_ && efeature_[e])) {
                epoint[e] = EdgeMidpoint(mesh, e);
            }

            // interior edge
            else {
                Vector<Real, 3> p(0, 0, 0);
                p += points_[mesh.get_vertex(e, 0)];
                p += points_[mesh.get_vertex(e, 1)];
                p += fpoint[mesh.get_face(e, 0)];
                p += fpoint[mesh.get_face(e, 1)];
                p *= 0.25f;
                epoint[e] = p;
            }
        }

        // compute new positions for old vertices
        for (auto v: mesh.vertices) {
            // isolated vertex?
            if (mesh.is_isolated(v)) {
                vpoint[v] = points_[v];
            }

            // boundary vertex?
            else if (mesh.is_boundary(v)) {
                if (boundary_handling == BoundaryHandling::Preserve) {
                    vpoint[v] = points_[v];
                } else {
                    auto h1 = mesh.get_halfedge(v);
                    auto h0 = mesh.get_prev(h1);

                    Vector<Real, 3> p = points_[v];
                    p *= 6.0;
                    p += points_[mesh.get_vertex(h1)];
                    p += points_[mesh.get_vertex(mesh.get_opposite(h0))];
                    p *= 0.125;

                    vpoint[v] = p;
                }
            }

            // interior feature vertex?
            else if (vfeature_ && vfeature_[v]) {
                Vector<Real, 3> p = points_[v];
                p *= 6.0;
                int count(0);

                for (auto h: mesh.get_halfedges(v)) {
                    if (efeature_[mesh.get_edge(h)]) {
                        p += points_[mesh.get_vertex(h)];
                        ++count;
                    }
                }

                if (count == 2) // vertex is on feature edge
                {
                    p *= 0.125;
                    vpoint[v] = p;
                } else // keep fixed
                {
                    vpoint[v] = points_[v];
                }
            }

            // interior vertex
            else {
                // weights from SIGGRAPH paper "Subdivision Surfaces in Character Animation"

                const Real k = mesh.get_valence(v);
                Vector<Real, 3> p(0, 0, 0);

                for (auto vv: mesh.get_vertices(v)) {
                    p += points_[vv];
                }

                for (auto f: mesh.get_faces(v)) {
                    p += fpoint[f];
                }

                p /= (k * k);

                p += ((k - 2.0f) / k) * points_[v];

                vpoint[v] = p;
            }
        }

        // assign new positions to old vertices
        for (auto v: mesh.vertices) {
            points_[v] = vpoint[v];
        }

        // split edges
        for (auto e: mesh.edges) {
            // feature edge?
            if (efeature_ && efeature_[e]) {
                auto h = insert_vertex(mesh, points_, e, epoint[e]);
                auto v = mesh.get_vertex(h);
                auto e0 = mesh.get_edge(h);
                auto e1 = mesh.get_edge(mesh.get_next(h));

                vfeature_[v] = true;
                efeature_[e0] = true;
                efeature_[e1] = true;
            }

            // normal edge
            else {
                insert_vertex(mesh, points_, e, epoint[e]);
            }
        }

        // split faces
        for (auto f: mesh.faces) {
            auto h0 = mesh.get_halfedge(f);
            mesh.insert_edge(h0, mesh.get_next(mesh.get_next(h0)));

            auto h1 = mesh.get_next(h0);
            insert_vertex(mesh, points_, mesh.get_edge(h1), fpoint[f]);

            auto h = mesh.get_next(mesh.get_next(mesh.get_next(h1)));
            while (h != h0) {
                mesh.insert_edge(h1, h);
                h = mesh.get_next(mesh.get_next(mesh.get_next(h1)));
            }
        }

        // clean-up properties
        mesh.remove_vertex_property(vpoint);
        mesh.remove_edge_property(epoint);
        mesh.remove_face_property(fpoint);
    }

    void Loop(Mesh &mesh, BoundaryHandling boundary_handling) {
        auto points_ = mesh.vertex_property<Vector<Real, 3> >("v:position");
        auto vfeature_ = mesh.get_vertex_property<bool>("v:feature");
        auto efeature_ = mesh.get_edge_property<bool>("e:feature");

        if (!mesh.is_triangle_mesh()) {
            auto what = std::string{__func__} + ": Not a triangle mesh.";
            throw std::invalid_argument(what);
        }

        // reserve memory
        size_t nv = mesh.n_vertices();
        size_t ne = mesh.n_edges();
        size_t nf = mesh.n_faces();
        mesh.reserve(nv + ne, 2 * ne + 3 * nf, 4 * nf);

        // add properties
        auto vpoint = mesh.add_vertex_property<Vector<Real, 3> >("loop:vpoint");
        auto epoint = mesh.add_edge_property<Vector<Real, 3> >("loop:epoint");

        // compute vertex positions
        for (auto v: mesh.vertices) {
            // isolated vertex?
            if (mesh.is_isolated(v)) {
                vpoint[v] = points_[v];
            }

            // boundary vertex?
            else if (mesh.is_boundary(v)) {
                if (boundary_handling == BoundaryHandling::Preserve) {
                    vpoint[v] = points_[v];
                } else {
                    auto h1 = mesh.get_halfedge(v);
                    auto h0 = mesh.get_prev(h1);

                    Vector<Real, 3> p = points_[v];
                    p *= 6.0;
                    p += points_[mesh.get_vertex(h1)];
                    p += points_[mesh.get_vertex(mesh.get_opposite(h0))];
                    p *= 0.125;
                    vpoint[v] = p;
                }
            }

            // interior feature vertex?
            else if (vfeature_ && vfeature_[v]) {
                Vector<Real, 3> p = points_[v];
                p *= 6.0;
                int count(0);

                for (auto h: mesh.get_halfedges(v)) {
                    if (efeature_[mesh.get_edge(h)]) {
                        p += points_[mesh.get_vertex(h)];
                        ++count;
                    }
                }

                if (count == 2) // vertex is on feature edge
                {
                    p *= 0.125;
                    vpoint[v] = p;
                } else // keep fixed
                {
                    vpoint[v] = points_[v];
                }
            }

            // interior vertex
            else {
                Vector<Real, 3> p(0, 0, 0);
                Real k(0);

                for (auto vv: mesh.get_vertices(v)) {
                    p += points_[vv];
                    ++k;
                }
                p /= k;

                Real beta = (0.625 - pow(0.375 + 0.25 * std::cos(2.0 * std::numbers::pi / k), 2.0));

                vpoint[v] = points_[v] * (Real) (1.0 - beta) + beta * p;
            }
        }

        // compute edge positions
        for (auto e: mesh.edges) {
            // boundary or feature edge?
            if (mesh.is_boundary(e) || (efeature_ && efeature_[e])) {
                epoint[e] = EdgeMidpoint(mesh, e);
            }

            // interior edge
            else {
                auto h0 = mesh.get_halfedge(e, 0);
                auto h1 = mesh.get_halfedge(e, 1);
                Vector<Real, 3> p = points_[mesh.get_vertex(h0)];
                p += points_[mesh.get_vertex(h1)];
                p *= 3.0;
                p += points_[mesh.get_vertex(mesh.get_next(h0))];
                p += points_[mesh.get_vertex(mesh.get_next(h1))];
                p *= 0.125;
                epoint[e] = p;
            }
        }

        // set new vertex positions
        for (auto v: mesh.vertices) {
            points_[v] = vpoint[v];
        }

        // insert new vertices on edges
        for (auto e: mesh.edges) {
            // feature edge?
            if (efeature_ && efeature_[e]) {
                auto h = insert_vertex(mesh, points_, e, epoint[e]);
                auto v = mesh.get_vertex(h);
                auto e0 = mesh.get_edge(h);
                auto e1 = mesh.get_edge(mesh.get_next(h));

                vfeature_[v] = true;
                efeature_[e0] = true;
                efeature_[e1] = true;
            }

            // normal edge
            else {
                insert_vertex(mesh, points_, e, epoint[e]);
            }
        }

        // split faces
        Halfedge h;
        for (auto f: mesh.faces) {
            h = mesh.get_halfedge(f);
            mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
            h = mesh.get_next(h);
            mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
            h = mesh.get_next(h);
            mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
        }

        // clean-up properties
        mesh.remove_vertex_property(vpoint);
        mesh.remove_edge_property(epoint);
    }

    void QuadTri(Mesh &mesh, BoundaryHandling boundary_handling) {
        auto points_ = mesh.vertex_property<Vector<Real, 3> >("v:position");

        // split each edge evenly into two parts
        for (auto e: mesh.edges) {
            insert_vertex(mesh, points_, e, EdgeMidpoint(mesh, e));
        }

        // subdivide faces without repositioning
        for (auto f: mesh.faces) {
            size_t f_val = mesh.get_valence(f) / 2;
            if (f_val == 3) {
                // face was a triangle
                Halfedge h0 = mesh.get_halfedge(f);
                Halfedge h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);

                h0 = mesh.get_next(h0);
                h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);

                h0 = mesh.get_next(h0);
                h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);
            } else {
                // quadrangulate the rest
                Halfedge h0 = mesh.get_halfedge(f);
                Halfedge h1 = mesh.get_next(mesh.get_next(h0));
                //NOTE: It's important to calculate the centroid before inserting the new edge
                auto cen = FaceCenter(mesh, f);
                h1 = mesh.insert_edge(h0, h1);
                insert_vertex(mesh, points_, mesh.get_edge(h1), cen);

                auto h =
                        mesh.get_next(mesh.get_next(mesh.get_next(h1)));
                while (h != h0) {
                    mesh.insert_edge(h1, h);
                    h = mesh.get_next(
                        mesh.get_next(mesh.get_next(h1)));
                }
            }
        }

        auto new_pos =
                mesh.add_vertex_property<Vector<Real, 3> >("quad_tri:new_position", Vector<Real, 3>::Zero());

        for (auto v: mesh.vertices) {
            if (mesh.is_boundary(v)) {
                if (boundary_handling == BoundaryHandling::Preserve) {
                    new_pos[v] = points_[v];
                } else {
                    new_pos[v] = 0.5 * points_[v];

                    // add neighboring vertices on boundary
                    for (auto vv: mesh.get_vertices(v)) {
                        if (mesh.is_boundary(vv)) {
                            new_pos[v] += 0.25 * points_[vv];
                        }
                    }
                }
            } else {
                // count the number of faces and quads surrounding the vertex
                int n_faces = 0;
                int n_quads = 0;
                for (auto f: mesh.get_faces(v)) {
                    n_faces++;
                    if (mesh.get_valence(f) == 4)
                        n_quads++;
                }

                if (n_quads == 0) {
                    // vertex is surrounded only by triangles
                    double a = 2.0 * pow(3.0 / 8.0 + (std::cos(2.0 * std::numbers::pi / n_faces) - 1.0) / 4.0, 2.0);
                    double b = (1.0 - a) / n_faces;

                    new_pos[v] = a * points_[v];
                    for (auto vv: mesh.get_vertices(v)) {
                        new_pos[v] += b * points_[vv];
                    }
                } else if (n_quads == n_faces) {
                    // vertex is surrounded only by quads
                    double c = (n_faces - 3.0) / n_faces;
                    double d = 2.0 / pow(n_faces, 2.0);
                    double e = 1.0 / pow(n_faces, 2.0);

                    new_pos[v] = c * points_[v];
                    for (auto h: mesh.get_halfedges(v)) {
                        new_pos[v] += d * points_[mesh.get_vertex(h)];
                        new_pos[v] += e * points_[mesh.get_vertex(mesh.get_next(h))];
                    }
                } else {
                    // vertex is surrounded by triangles and quads
                    double alpha = 1.0 / (1.0 + 0.5 * n_faces + 0.25 * n_quads);
                    double beta = 0.5 * alpha;
                    double gamma = 0.25 * alpha;

                    new_pos[v] = alpha * points_[v];
                    for (auto h: mesh.get_halfedges(v)) {
                        new_pos[v] += beta * points_[mesh.get_vertex(h)];
                        if (mesh.get_valence(mesh.get_face(h)) == 4) {
                            new_pos[v] += gamma * points_[mesh.get_vertex(mesh.get_next(h))];
                        }
                    }
                }
            }
        }

        // apply new positions to the mesh
        for (auto v: mesh.vertices) {
            points_[v] = new_pos[v];
        }

        mesh.remove_vertex_property(new_pos);
    }

    void Linear(Mesh &mesh) {
        auto points_ = mesh.vertex_property<Vector<Real, 3> >("v:position");
        // linear subdivision of edges
        for (auto e: mesh.edges) {
            insert_vertex(mesh, points_, e, EdgeMidpoint(mesh, e));
        }

        // subdivide faces
        for (auto f: mesh.faces) {
            size_t f_val = mesh.get_valence(f) / 2;

            if (f_val == 3) // triangle
            {
                Halfedge h0 = mesh.get_halfedge(f);
                Halfedge h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);

                h0 = mesh.get_next(h0);
                h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);

                h0 = mesh.get_next(h0);
                h1 = mesh.get_next(mesh.get_next(h0));
                mesh.insert_edge(h0, h1);
            } else // quadrangulate other faces
            {
                Halfedge h0 = mesh.get_halfedge(f);
                Halfedge h1 = mesh.get_next(mesh.get_next(h0));

                // NOTE: It's important to calculate the centroid before inserting the new edge
                auto cen = FaceCenter(mesh, f);
                h1 = mesh.insert_edge(h0, h1);
                insert_vertex(mesh, points_, mesh.get_edge(h1), cen);

                auto h = mesh.get_next(mesh.get_next(mesh.get_next(h1)));
                while (h != h0) {
                    mesh.insert_edge(h1, h);
                    h = mesh.get_next(
                        mesh.get_next(mesh.get_next(h1)));
                }
            }
        }
    }
}
