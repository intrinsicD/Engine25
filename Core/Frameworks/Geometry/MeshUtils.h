//
// Created by alex on 2/5/25.
//

#ifndef MESHUTILS_H
#define MESHUTILS_H

#include "Mesh.h"
#include "TriangleUtils.h"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------
    // Mesh Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] bool ValidateMesh(const Mesh &mesh);

    [[nodiscard]] Mesh Dual(const Mesh &mesh);

    [[nodiscard]] Real
    VolumeTetrahedralDecomposition(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    template<typename T, int N>
    [[nodiscard]] Real SurfaceArea(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions) {
        double area = 0;
        for (const auto &f: mesh.faces) {
            area += FaceArea(mesh, positions, f);
        }
        return area;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------


    [[nodiscard]] Vector<Real, 3>
    VertexNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v);

    template<typename T>
    T ClampCotan(T v) {
        constexpr T bound = 19.1;
        return (v < -bound ? -bound : (v > bound ? bound : v));
    }

    template<typename T, int N>
    [[nodiscard]] Real
    VertexVoronoiMixedArea(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Vertex &v) {
        double area = 0.0;
        constexpr double epsilon = std::numeric_limits<double>::epsilon();

        if (!mesh.is_isolated(v)) {
            const Vector<double, N> p = positions[v].template cast<double>();

            for (const auto &h: mesh.get_halfedges(v)) {
                const Halfedge h0 = h;
                const Halfedge h1 = mesh.get_next(h0);

                if (mesh.is_boundary(h0)) {
                    continue;
                }

                // three vertex positions
                const Vector<Real, N> &q = positions[mesh.get_vertex(h0)];
                const Vector<Real, N> &r = positions[mesh.get_vertex(h1)];

                // edge vectors
                const Vector<double, N> pq = q.template cast<double>() - p;
                const Vector<double, N> qr = r.template cast<double>() - q.template cast<double>();
                const Vector<double, N> pr = r.template cast<double>() - p;

                double a = pq.norm();
                double b = qr.norm();
                double c = pr.norm();

                // compute and check triangle area
                const double twiceTriArea = TriangleAreaHeron(a, b,
                                                              c); // its twice the triangle area to avoid computation here and in the following...
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

    template<typename T, int N>
    [[nodiscard]] Real
    VertexBarycentricArea(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Vertex &v) {
        double area = 0;
        if (!mesh.is_isolated(v)) {
            for (const auto &f: mesh.get_faces(v)) {
                area += FaceArea(mesh, positions, f);
            }
            area /= 3.0;
        }
        return area;
    }

    template<typename T, int N>
    [[nodiscard]] Vector<T, N>
    VertexCenter(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Vertex &v) {
        Vector<double, N> center = Vector<double, N>::Zero();

        if (!mesh.is_isolated(v)) {
            int count = 0;
            for (const auto &vv: mesh.get_vertices(v)) {
                center += positions[vv].template cast<double>();
                ++count;
            }
            center /= count;
        }
        return center.template cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3>
    VertexStarGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v,
                       VertexProperty<Real> scalarfield);

    template<typename T, int N>
    [[nodiscard]] Vector<T, N>
    VertexLaplace(const Mesh &mesh,
                  const VertexProperty<Vector<T, N>> &positions,
                  const HalfedgeProperty<Real> &halfedge_weight, const Vertex &v, Real vertex_area) {
        Vector<double, N> laplace = Vector<double, N>::Zero();

        if (!mesh.is_isolated(v)) {
            double sum_weights = 0.0;

            for (auto h: mesh.get_halfedges(v)) {
                double weight = halfedge_weight[h];
                sum_weights += weight;
                laplace += weight * positions[mesh.get_vertex(h)].template cast<double>();
            }

            laplace -= sum_weights * positions[v].template cast<double>();
            laplace /= (vertex_area > 0 ? 2.0 * vertex_area : sum_weights);
        }

        return laplace.template cast<T>();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------

    template<typename T, int N>
    Real FaceArea(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Face &f) {
        auto fh = mesh.get_halfedges(f);
        Vertex p0 = mesh.get_vertex(mesh.get_opposite(*fh));
        Vertex p1 = mesh.get_vertex(*fh);
        ++fh;
        double area = 0.0;

        for (const auto &hn: fh) {
            Vertex p2 = mesh.get_vertex(hn);

            double a = (positions[p1] - positions[p0]).template cast<double>().norm();
            double b = (positions[p2] - positions[p0]).template cast<double>().norm();
            double c = (positions[p1] - positions[p2]).template cast<double>().norm();

            area += TriangleAreaHeron(a, b, c);

            p1 = p2;
        }
        return area;
    }

    template<typename T, int N>
    Vector<T, N> FaceCenter(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Face &f) {
        Vector<double, N> center = Vector<double, N>::Zero();
        int count = 0;
        for (const auto &v: mesh.get_vertices(f)) {
            center += positions[v].template cast<double>();
            ++count;
        }
        return (center / count).template cast<T>();
    }

    [[nodiscard]] Vector<Real, 3>
    FaceAreaVector(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f);

    [[nodiscard]] Vector<Real, 3>
    FaceNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f);

    [[nodiscard]] Vector<Real, 3>
    FaceGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                 VertexProperty<Real> scalarfield);

    [[nodiscard]] Vector<Real, 3>
    FaceToBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                                 const Vector<Real, 3> &p);

    [[nodiscard]] Vector<Real, 3>
    FaceFromBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f,
                                   const Vector<Real, 3> &bc);

    //------------------------------------------------------------------------------------------------------------------
    // Edge Methods
    //------------------------------------------------------------------------------------------------------------------

    template<typename T, int N>
    [[nodiscard]] Real EdgeLength(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return static_cast<Real>((positions[mesh.get_vertex(e, 1)].template cast<double>() - positions[mesh.
                get_vertex(e, 0)].template cast<double>()).norm());
    }

    template<typename T, int N>
    [[nodiscard]] Vector<Real, 3>
    EdgeVector(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return (positions[mesh.get_vertex(e, 1)].template cast<double>() -
                positions[mesh.get_vertex(e, 0)].template cast<double>()).template cast<Real>();
    }

    template<typename T, int N>
    [[nodiscard]] Vector<Real, 3>
    EdgeMidpoint(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return ((positions[mesh.get_vertex(e, 0)].template cast<double>() +
                 positions[mesh.get_vertex(e, 1)].template cast<double>()) / 2.0).template cast<Real>();
    }

    template<typename T, int N>
    [[nodiscard]] Real EdgeCotan(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        double weight = 0.0;

        const auto h0 = mesh.get_halfedge(e, 0);
        const auto h1 = mesh.get_halfedge(e, 1);

        const Vector<T, N> &p0 = positions[mesh.get_vertex(h0)];
        const Vector<T, N> &p1 = positions[mesh.get_vertex(h1)];

        if (!mesh.is_boundary(h0)) {
            const Vector<T, N> p2 = positions[mesh.get_vertex(mesh.get_next(h0))];
            const Vector<double, 3> d0 = p0.template cast<double>() - p2.template cast<double>();
            const Vector<double, 3> d1 = p1.template cast<double>() - p2.template cast<double>();

            double a = (d0.norm());
            double b = (d1.norm());
            double c = (p0 - p1).norm();
            double area = 2.0 * TriangleAreaHeron(a, b, c);
            if (area > std::numeric_limits<double>::min()) {
                const double cot = d0.dot(d1) / area;
                // weight += clamp_cot(cot);
                weight += cot;
            }
        }

        if (!mesh.is_boundary(h1)) {
            const Vector<T,N> p2 = positions[mesh.get_vertex(mesh.get_next(h1))];
            const Vector<double, 3> d0 = p0.template cast<double>() - p2.template cast<double>();
            const Vector<double, 3> d1 = p1.template cast<double>() - p2.template cast<double>();

            double a = (d0.norm());
            double b = (d1.norm());
            double c = (p0 - p1).norm();
            double area = 2.0 * TriangleAreaHeron(a, b, c);

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



}

#endif //MESHUTILS_H
