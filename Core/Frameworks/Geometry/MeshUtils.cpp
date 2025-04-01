//
// Created by alex on 2/5/25.
//

#include "MeshUtils.h"
#include "TriangleUtils.h"
#include "Eigen/Geometry"
#include <iostream>

namespace Bcg {

    Vector<Real, 3>
    PolygonalFaceAreaVector(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f) {
        Vector<double, 3> vector_area = Vector<double, 3>::Zero();
        for (const auto &h: mesh.get_halfedges(f)) {
            Vertex v1 = mesh.get_vertex(h);
            Vertex v0 = mesh.get_vertex(mesh.get_opposite(h));
            vector_area += positions[v0].template cast<double>().cross(positions[v1].template cast<double>()) / 2.0;
        }
        return vector_area.cast<Real>();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Mesh Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] bool ValidateMesh(const Mesh &mesh) {
        // Validate all faces: each face should have a closed halfedge cycle.
        for (const Face &f: mesh.faces) {
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
                    std::cerr << "Error: Halfedge cycle for face " << f.idx() <<
                              " did not close properly (possible infinite loop).\n";
                    return false;
                }
            } while (h != h0);
        }

        // Validate halfedge opposites.
        for (const Halfedge &h: mesh.halfedges) {
            if (!h.is_valid())
                continue;
            Halfedge opp = mesh.get_opposite(h);
            if (!opp.is_valid() || mesh.get_opposite(opp) != h) {
                std::cerr << "Error: Halfedge " << h.idx() << " has an invalid opposite.\n";
                return false;
            }
        }

        // Validate that each vertex's halfedge pointer is consistent.
        for (const Vertex &v: mesh.vertices) {
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
        auto positions = dual.vertex_property<Vector<Real, 3>>("v:position");

        for (const auto &f: mesh.faces) {
            add_vertex(dual.vertices, positions, FaceCenter(mesh, positions, f));
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

    [[nodiscard]] Real
    VolumeTetrahedralDecomposition(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions) {
        double volume = 0.0;
        // Loop over all faces in the mesh.
        for (const Face &f: mesh.faces) {
            // Get a circulator for the halfedges around the face.
            auto h0 = mesh.get_halfedge(f); // starting halfedge
            auto h = h0; // copy to iterate

            // Compute p0 from the starting vertex.
            Vertex v0 = mesh.get_vertex(h0);
            Vector<double, 3> p0 = positions[v0].template cast<double>();

            // Advance h to the second halfedge.
            h = mesh.get_next(h);

            // Continue fan-triangulation until the next halfedge would close the loop.
            while (mesh.get_next(h) != h0) {
                Vertex v1 = mesh.get_vertex(h);
                Vertex v2 = mesh.get_vertex(mesh.get_next(h));

                Vector<double, 3> p1 = positions[v1].template cast<double>();
                Vector<double, 3> p2 = positions[v2].template cast<double>();

                // Compute the signed volume of the tetrahedron (p0, p1, p2, origin).
                double tet_volume = p0.dot(p1.cross(p2)) / 6.0;
                volume += tet_volume;

                // Move to the next halfedge in the face.
                h = mesh.get_next(h);
            }
        }
        // For a closed, consistently oriented mesh, the signed volume should be positive.
        // Here we take the absolute value to return a non-negative volume.
        return std::abs(volume);
    }


    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions) {
        double volume = 0.0;
        // Loop over all faces in the mesh.
        for (const Face &f: mesh.faces) {
            // Compute the face area vector (assumes your function returns a Vector<double,3>).
            Vector<double, 3> fav = PolygonalFaceAreaVector(mesh, positions, f).template cast<double>();
            double face_area = fav.norm();
            if (face_area == 0)
                continue; // Skip degenerate faces.

            // Compute the face normal (ensure it points outward).
            Vector<double, 3> normal = fav / face_area;

            // Compute the face centroid.
            Vector<double, 3> centroid = FaceCenter(mesh, positions, f).template cast<double>();

            // Contribution of the face to the volume.
            // (centroid dot normal) gives the signed distance from the origin.
            volume += (centroid.dot(normal) * face_area) / 3.0;
        }
        return std::abs(volume);
    }


    [[nodiscard]] Real SurfaceArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions) {
        double area = 0;
        for (const auto &f: mesh.faces) {
            area += FaceArea(mesh, positions, f);
        }
        return area;
    }


    template<typename T, int N>
    [[nodiscard]] Vector<T, N> Centroid(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions) {
        Vector<double, N> center = Vector<double, N>::Zero();
        for (const auto &v: mesh.vertices) {
            center += positions[v].template cast<double>();
        }
        return (center / mesh.vertices.size()).template cast<T>();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------


    Real FaceArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f) {
        auto fv = mesh.get_vertices(f);
        Vertex p0 = *fv;
        ++fv;
        double area = 0.0;
        for (const auto &v: fv) {
            Vector<double, 3> d = positions[v].template cast<double>() - positions[p0].template cast<double>();
            Vector<double, 3> e =
                    positions[fv.get_next()].template cast<double>() - positions[p0].template cast<double>();
            area += d.cross(e).norm();
        }
        return area / 2.0;
        //return static_cast<Real>(PolygonalFaceAreaVector<double>(mesh, f).norm());
    }

    [[nodiscard]] Vector<Real, 3>
    FaceAreaVector(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f) {
        return PolygonalFaceAreaVector(mesh, positions, f);
    }


    [[nodiscard]] Vector<Real, 3>
    FaceNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f) {
        return PolygonalFaceAreaVector(mesh, positions, f).normalized();
    }

    [[nodiscard]] Vector<Real, 3>
    FaceGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                 VertexProperty<Real> scalarfield) {
        double max_mag = 0;
        for (const auto &h: mesh.get_halfedges(f)) {
            max_mag = std::max(max_mag, std::abs(static_cast<double>(scalarfield[mesh.get_vertex(mesh.get_next(h))])));
        }
        Vector<double, 3> fav = PolygonalFaceAreaVector(mesh, positions, f).template cast<double>();
        double f_area = fav.norm();
        Vector<double, 3> normal = fav / f_area;

        Vector<double, 3> grad = Vector<double, 3>::Zero();
        for (const auto &h: mesh.get_halfedges(f)) {
            Vector<double, 3> ev = positions[mesh.get_vertex(h)].template cast<double>() - positions[mesh.
                    get_vertex(mesh.get_opposite(h))].template cast<double>();
            double u = scalarfield[mesh.get_vertex(mesh.get_next(h))];
            grad += normal.cross(ev) * u / max_mag;
        }
        return ((grad / (2.0 * f_area)) * max_mag).cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3>
    FaceToBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                                 const Vector<Real, 3> &p) {
        const Halfedge h = mesh.get_halfedge(f);
        const Halfedge hn = mesh.get_next(h);
        const Halfedge hnn = mesh.get_next(hn);
        const Vector<Real, 3> &u = positions[mesh.get_vertex(h)];
        const Vector<Real, 3> &v = positions[mesh.get_vertex(hn)];
        const Vector<Real, 3> &w = positions[mesh.get_vertex(hnn)];
        return ToBarycentricCoordinates(p, u, v, w);
    }

    [[nodiscard]] Vector<Real, 3>
    FaceFromBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                                   const Vector<Real, 3> &bc) {
        const Halfedge h = mesh.get_halfedge(f);
        const Halfedge hn = mesh.get_next(h);
        const Halfedge hnn = mesh.get_next(hn);
        const Vector<Real, 3> &u = positions[mesh.get_vertex(h)];
        const Vector<Real, 3> &v = positions[mesh.get_vertex(hn)];
        const Vector<Real, 3> &w = positions[mesh.get_vertex(hnn)];
        return FromBarycentric(bc, u, v, w);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Edge Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Real EdgeCotan(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Edge &e) {
        double weight = 0.0;

        const auto h0 = mesh.get_halfedge(e, 0);
        const auto h1 = mesh.get_halfedge(e, 1);

        const Vector<double, 3> p0 = positions[mesh.get_vertex(h0)].cast<double>();
        const Vector<double, 3> p1 = positions[mesh.get_vertex(h1)].cast<double>();

        if (!mesh.is_boundary(h0)) {
            const Vector<double, 3> p2 = positions[mesh.get_vertex(mesh.get_next(h0))].cast<double>();
            const Vector<double, 3> d0 = p0 - p2;
            const Vector<double, 3> d1 = p1 - p2;
            const double area = d0.cross(d1).norm(); //triangle area * 2
            if (area > std::numeric_limits<double>::min()) {
                const double cot = d0.dot(d1) / area;
                // weight += clamp_cot(cot);
                weight += cot;
            }
        }

        if (!mesh.is_boundary(h1)) {
            const Vector<double, 3> p2 = positions[mesh.get_vertex(mesh.get_next(h1))].cast<double>();
            const Vector<double, 3> d0 = p0 - p2;
            const Vector<double, 3> d1 = p1 - p2;
            const double area = d0.cross(d1).norm(); //triangle area * 2
            if (area > std::numeric_limits<double>::min()) {
                const double cot = d0.dot(d1) / area;
                // weight += clamp_cot(cot);
                weight += cot;
            }
        }

        assert(!std::isnan(weight));
        assert(!std::isinf(weight));

        return weight;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------


    [[nodiscard]] Vector<Real, 3>
    VertexNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v) {
        Vector<double, 3> v_normal = Vector<double, 3>::Zero();
        if (!mesh.is_isolated(v)) {
            for (const auto &f: mesh.get_faces(v)) {
                v_normal += PolygonalFaceAreaVector(mesh, positions, f).cast<double>();
            }
            v_normal /= v_normal.norm();
        }
        return v_normal.cast<Real>();
    }

    inline double Cot(double angle) {
        return std::cos(angle) / std::sin(angle);
    }


    Real ComputeCot(const Vector<Real, 3> &a, const Vector<Real, 3> &b) {
        return a.dot(b) / std::max(double(a.cross(b).norm()), 1e-8);
    }

    template<typename T>
    T ClampCotan(T v) {
        constexpr T bound = 19.1;
        return (v < -bound ? -bound : (v > bound ? bound : v));
    }

    [[nodiscard]] Real
    VertexVoronoiMixedArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v) {
        double area = 0.0;
        constexpr double epsilon = std::numeric_limits<double>::epsilon();

        if (!mesh.is_isolated(v)) {
            const Vector<double, 3> p = positions[v].cast<double>();

            for (const auto &h: mesh.get_halfedges(v)) {
                const Halfedge h0 = h;
                const Halfedge h1 = mesh.get_next(h0);

                if (mesh.is_boundary(h0)) {
                    continue;
                }

                // three vertex positions
                const Vector<double, 3> q = positions[mesh.get_vertex(h0)].cast<double>();
                const Vector<double, 3> r = positions[mesh.get_vertex(h1)].cast<double>();

                // edge vectors
                const Vector<double, 3> pq = q - p;
                const Vector<double, 3> qr = r - q;
                const Vector<double, 3> pr = r - p;

                // compute and check triangle area
                //const double triArea = PolygonalFaceAreaVector<double>(mesh, mesh.get_face(h)).norm();
                const double twiceTriArea = pq.cross(
                        pr).norm(); // its twice the triangle area to avoid computation here and in the following...
                if (twiceTriArea <= epsilon) {
                    continue;
                }
                //assert(triArea > 0.0 && std::abs(triArea - triArea_check) < epsilon);
                // dot products for each corner (of its two emanating edge vectors)
                const double dotp = pq.dot(pr);
                const double dotq = -pq.dot(qr);
                const double dotr = qr.dot(pr);

                if (dotp < 0.0) {
                    // angle at p is obtuse
                    area += twiceTriArea / 4.0; // corresponds to triArea / 2.0
                } else if (dotq < 0.0 || dotr < 0.0) {
                    // angle at q or r obtuse
                    area += twiceTriArea / 8.0; // corresponds to triArea / 4.0
                } else {
                    // no obtuse angles
                    // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                    const double cotq = dotq / twiceTriArea;
                    const double cotr = dotr / twiceTriArea;

                    // clamp cot(angle) by clamping angle to [1,179]
                    area += (pr.squaredNorm() * ClampCotan(cotq) + pq.squaredNorm() * ClampCotan(cotr)) / 8.0;
                }
            }
        }
        return area;
    }

    [[nodiscard]] Real
    VertexBarycentricArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v) {
        double area = 0;
        if (!mesh.is_isolated(v)) {
            for (const auto &f: mesh.get_faces(v)) {
                //area += PolygonalFaceAreaVector<double>(mesh, f).norm();
                area += FaceArea(mesh, positions, f);
            }
            area /= 3.0;
        }
        return area;
    }

    [[nodiscard]] Vector<Real, 3>
    VertexStarGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v,
                       VertexProperty<Real> scalarfield) {
        Vector<double, 3> gradient = Vector<double, 3>::Zero();

        if (!mesh.is_isolated(v)) {
            double sum_areas = 0;
            for (const auto &f: mesh.get_faces(v)) {
                Vector<double, 3> gradient = FaceGradient(mesh, positions, f, scalarfield).cast<double>();
                double area = PolygonalFaceAreaVector(mesh, positions, f).norm();
                gradient += gradient * area;
                sum_areas += area;
            }

            gradient /= sum_areas;
        }
        return gradient.cast<Real>();
    }


}
