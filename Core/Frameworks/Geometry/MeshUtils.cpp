//
// Created by alex on 2/5/25.
//

#include "MeshUtils.h"
#include "Eigen/Geometry"
#include <iostream>

namespace Bcg {
    template<typename T>
    Vector<T, 3> PolygonalFaceAreaVector(const Mesh &mesh, const Face &f) {
        Vector<T, 3> vector_area = Vector<T, 3>::Zero();
        for (const auto &h: mesh.get_halfedges(f)) {
            Vertex v1 = mesh.get_vertex(h);
            Vertex v0 = mesh.get_vertex(mesh.get_opposite(h));
            vector_area += mesh.positions[v0].cast<T>().cross(mesh.positions[v1].cast<T>()) / 2.0;
        }
        return vector_area;
    }

    template<typename T>
    Vector<T, 3> Mean(const Mesh &mesh, const Face &f) {
        Vector<T, 3> center = Vector<T, 3>::Zero();
        int count = 0;
        for (const auto &v: mesh.get_vertices(f)) {
            center += mesh.positions[v].cast<T>();
            ++count;
        }
        return center / count;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Mesh Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] bool ValidateMesh(const Mesh &mesh) {
        // Validate all faces: each face should have a closed halfedge cycle.
        for (const Face &f : mesh.faces) {
            Halfedge h0 = mesh.get_halfedge(f);
            if (!h0.is_valid()) {
                std::cerr << "Error: Face " << f.idx() << " has no valid starting halfedge.\n";
                return false;
            }
            Halfedge h = h0;
            int count = 0;
            const int max_iter = mesh.faces.size() * 10; // a safeguard against infinite loops
            do {
                // Check that the halfedge's face pointer matches the current face.
                auto h_face = mesh.get_face(h);
                if (h_face != f) {
                    std::cerr << "Error: Inconsistent face pointer for halfedge " << h.idx()
                         << " in face " << f.idx()
                         << ". Expected face " << f.idx()
                         << " but got face " << h_face.idx() << std::endl;

                    auto opp = mesh.get_opposite(h);
                    std::cerr << "Info: Halfedge " << h.idx() << " (opp " << opp.idx() << ") in face " << f.idx()
                              << " has face pointer " << mesh.get_face(h).idx() << std::endl;
                    std::cerr << "Info: Halfedge opp " << opp.idx() << " in face " << f.idx()
                     << " has face pointer " << mesh.get_face(opp).idx() << std::endl;
                    return false;
                }
                h = mesh.get_next(h);
                ++count;
                if (count > max_iter) {
                    std::cerr << "Error: Halfedge cycle for face " << f.idx() << " did not close properly (possible infinite loop).\n";
                    return false;
                }
            } while (h != h0);
        }

        // Validate halfedge opposites.
        for (const Halfedge &h : mesh.halfedges) {
            if (!h.is_valid())
                continue;
            Halfedge opp = mesh.get_opposite(h);
            if (!opp.is_valid() || mesh.get_opposite(opp) != h) {
                std::cerr << "Error: Halfedge " << h.idx() << " has an invalid opposite.\n";
                return false;
            }
        }

        // Validate that each vertex's halfedge pointer is consistent.
        for (const Vertex &v : mesh.vertices) {
            Halfedge h = mesh.get_halfedge(v);
            if (h.is_valid() && mesh.get_vertex(mesh.get_opposite(h)) != v) {
                std::cerr << "Error: Vertex " << v.idx() << " has an inconsistent halfedge pointer.\n";
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] Mesh Dual(const Mesh &mesh) {
        Mesh dual;

        for (const auto &f: mesh.faces) {
            dual.add_vertex(FaceCenter(mesh, f));
        }

        for (const auto &v: mesh.vertices) {
            std::vector<Vertex> indices;
            for (const auto &f: mesh.get_faces(v)) {
                indices.emplace_back(f.idx());
            }
            dual.add_face(indices);
        }
        return dual;
    }

    [[nodiscard]] Real VolumeTetrahedralDecomposition(const Mesh &mesh) {
        double volume = 0.0;
        // Loop over all faces in the mesh.
        for (const Face &f: mesh.faces) {
            // Get a circulator for the halfedges around the face.
            auto h0 = mesh.get_halfedge(f); // starting halfedge
            auto h = h0; // copy to iterate

            // Compute p0 from the starting vertex.
            Vertex v0 = mesh.get_vertex(h0);
            Vector<double, 3> p0 = mesh.positions[v0].cast<double>();

            // Advance h to the second halfedge.
            h = mesh.get_next(h);

            // Continue fan-triangulation until the next halfedge would close the loop.
            while (mesh.get_next(h) != h0) {
                Vertex v1 = mesh.get_vertex(h);
                Vertex v2 = mesh.get_vertex(mesh.get_next(h));

                Vector<double, 3> p1 = mesh.positions[v1].cast<double>();
                Vector<double, 3> p2 = mesh.positions[v2].cast<double>();

                // Compute the signed volume of the tetrahedron (p0, p1, p2, origin).
                double tet_volume = p0.dot(p1.cross(p2)) / 6.0;
                volume += tet_volume;

                // Move to the next halfedge in the face.
                h = mesh.get_next(h);
            }
        }
        // For a closed, consistently oriented mesh, the signed volume should be positive.
        // Here we take the absolute value to return a non-negative volume.
        return static_cast<Real>(std::abs(volume));
    }

    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh) {
        double volume = 0.0;
        // Loop over all faces in the mesh.
        for (const Face &f: mesh.faces) {
            // Compute the face area vector (assumes your function returns a Vector<double,3>).
            Vector<double, 3> fav = PolygonalFaceAreaVector<double>(mesh, f);
            double face_area = fav.norm();
            if (face_area == 0)
                continue; // Skip degenerate faces.

            // Compute the face normal (ensure it points outward).
            Vector<double, 3> normal = fav / face_area;

            // Compute the face centroid.
            Vector<double, 3> centroid = Mean<double>(mesh, f);

            // Contribution of the face to the volume.
            // (centroid dot normal) gives the signed distance from the origin.
            volume += (centroid.dot(normal) * face_area) / 3.0;
        }
        return static_cast<Real>(std::abs(volume));
    }

    [[nodiscard]] Real SurfaceArea(const Mesh &mesh) {
        double area = 0;
        for (const auto &f: mesh.faces) {
            area += FaceArea(mesh, f);
        }
        return area;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------

    Real FaceArea(const Mesh &mesh, const Face &f) {
        return static_cast<Real>(PolygonalFaceAreaVector<double>(mesh, f).norm());
    }

    Vector<Real, 3> FaceCenter(const Mesh &mesh, const Face &f) {
        return Mean<double>(mesh, f).cast<Real>();
    }

    Vector<Real, 3> FaceAreaVector(const Mesh &mesh, const Face &f) {
        return PolygonalFaceAreaVector<double>(mesh, f).cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3> FaceNormal(const Mesh &mesh, const Face &f) {
        return PolygonalFaceAreaVector<double>(mesh, f).normalized().cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3> FaceGradient(const Mesh &mesh, const Face &f, VertexProperty<Real> scalarfield) {
        double max_mag = 0;
        for (const auto &h: mesh.get_halfedges(f)) {
            max_mag = std::max(max_mag, std::abs(static_cast<double>(scalarfield[mesh.get_vertex(mesh.get_next(h))])));
        }
        Vector<double, 3> fav = PolygonalFaceAreaVector<double>(mesh, f);
        double f_area = fav.norm();
        Vector<double, 3> normal = fav / f_area;

        Vector<double, 3> grad = Vector<double, 3>::Zero();
        for (const auto &h: mesh.get_halfedges(f)) {
            Vector<double, 3> ev = mesh.positions[mesh.get_vertex(h)].cast<double>() - mesh.positions[mesh.
                                       get_vertex(mesh.get_opposite(h))].cast<double>();
            double u = scalarfield[mesh.get_vertex(mesh.get_next(h))];
            grad += normal.cross(ev) * u / max_mag;
        }
        return ((grad / (2.0 * f_area)) * max_mag).cast<Real>();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Vector<Real, 3> VertexNormal(const Mesh &mesh, const Vertex &v) {
        Vector<double, 3> v_normal = Vector<double, 3>::Zero();
        for (const auto &f: mesh.get_faces(v)) {
            v_normal += PolygonalFaceAreaVector<double>(mesh, f);
        }
        return v_normal.normalized().cast<Real>();
    }

    inline double Cot(double angle) {
        return std::cos(angle) / std::sin(angle);
    }

    template<typename T>
    T ComputeCot(const Vector<T, 3> &a, const Vector<T, 3> &b) {
        return a.dot(b) / std::max(a.cross(b).norm(), 1e-8);
    }

    template<typename T>
    T ClampCotan(T v) {
        constexpr T bound = 19.1;
        return (v < -bound ? -bound : (v > bound ? bound : v));
    }

    [[nodiscard]] Real VertexVoronoiArea(const Mesh &mesh, const Vertex &v) {
        double area = 0.0;
        constexpr double epsilon = std::numeric_limits<double>::epsilon();

        if (!mesh.is_isolated(v)) {
            for (const auto &h: mesh.Graph::get_halfedges(v)) {
                const Halfedge h0 = h;
                const Halfedge h1 = mesh.get_next(h0);
                const Halfedge h2 = mesh.get_next(h1);

                if (mesh.is_boundary(h0)) {
                    continue;
                }

                // three vertex positions
                const Vector<double, 3> p = mesh.positions[mesh.get_vertex(h2)].cast<double>();
                const Vector<double, 3> q = mesh.positions[mesh.get_vertex(h0)].cast<double>();
                const Vector<double, 3> r = mesh.positions[mesh.get_vertex(h1)].cast<double>();

                // edge vectors
                const Vector<double, 3> pq = q - p;
                const Vector<double, 3> qr = r - q;
                const Vector<double, 3> pr = r - p;

                // compute and check triangle area
                const double triArea = PolygonalFaceAreaVector<double>(mesh, mesh.get_face(h)).norm();
                const double triArea_check = pq.cross(qr).norm() / 2.0;
                assert(triArea > 0.0 && std::abs(triArea - triArea_check) < epsilon);
                if (triArea <= epsilon) {
                    continue;
                }
                // dot products for each corner (of its two emanating edge vectors)
                const double dotp = pq.dot(pr);
                const double dotq = -qr.dot(pq);
                const double dotr = qr.dot(pr);

                if (dotp < 0.0) {
                    // angle at p is obtuse
                    area += 0.25 * triArea;
                } else if (dotq < 0.0 || dotr < 0.0) {
                    // angle at q or r obtuse
                    area += 0.5 * triArea;
                } else {
                    // no obtuse angles
                    // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                    const double cotq = dotq / triArea;
                    const double cotr = dotr / triArea;

                    // clamp cot(angle) by clamping angle to [1,179]
                    area += 0.125 * (pr.squaredNorm() * ClampCotan(cotq) + pq.squaredNorm() * ClampCotan(cotr));
                }
            }
        }
        return area;
    }

    [[nodiscard]] Real VertexBarycentricArea(const Mesh &mesh, const Vertex &v) {
        double area = 0;
        for (const auto &f: mesh.get_faces(v)) {
            area += PolygonalFaceAreaVector<double>(mesh, f).norm();
        }
        return area / 3.0;
    }

    [[nodiscard]] Vector<Real, 3> VertexStarCenter(const Mesh &mesh, const Vertex &v) {
        Vector<double, 3> center = Vector<double, 3>::Zero();
        int count = 0;
        for (const auto &vv: mesh.get_vertices(v)) {
            center += mesh.positions[vv].cast<double>();
            ++count;
        }
        return (center / count).cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3> VertexStarGradient(const Mesh &mesh, const Vertex &v,
                                                     VertexProperty<Real> scalarfield) {
        Vector<double, 3> sum_gradients = Vector<double, 3>::Zero();
        double sum_areas = 0;
        for (const auto &f: mesh.get_faces(v)) {
            Vector<double, 3> gradient = FaceGradient(mesh, f, scalarfield).cast<double>();
            double area = PolygonalFaceAreaVector<double>(mesh, f).norm();
            sum_gradients += gradient * area;
            sum_areas += area;
        }
        return (sum_gradients / sum_areas).cast<Real>();
    }
}
