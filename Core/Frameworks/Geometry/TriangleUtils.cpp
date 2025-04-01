//
// Created by alex on 2/10/25.
//

#include "TriangleUtils.h"
#include "Eigen/Geometry"

namespace Bcg {
    Real TriangleArea(const Vector<Real, 3> &a, const Vector<Real, 3> &b, const Vector<Real, 3> &c) {
        Vector<Real, 3> ab = b - a;
        Vector<Real, 3> ac = c - a;
        return ab.cross(ac).norm() / 2.0;
    }

    double TriangleAreaHeron( double a, double b, double c){
        if (a < b) std::swap(a, b);
        if (a < c) std::swap(a, c);
        if (b < c) std::swap(b, c);

        return std::sqrt(std::abs((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)))) / 4.0;
    }

    Vector<Real, 3> ToBarycentricCoordinates(const Vector<Real, 3> &p, const Vector<Real, 3> &u,
                                             const Vector<Real, 3> &v,
                                             const Vector<Real, 3> &w) {
        Vector<double, 3> result = Vector<double, 3>::Constant(1.0 / 3.0); // default: barycenter
        Vector<double, 3> vu = v.cast<double>() - u.cast<double>();
        Vector<double, 3> wu = w.cast<double>() - u.cast<double>();
        Vector<double, 3> pu = p.cast<double>() - u.cast<double>();

        // find largest absolute coordinate of normal
        double nx = vu[1] * wu[2] - vu[2] * wu[1];
        double ny = vu[2] * wu[0] - vu[0] * wu[2];
        double nz = vu[0] * wu[1] - vu[1] * wu[0];
        double ax = abs(nx);
        double ay = abs(ny);
        double az = abs(nz);

        unsigned char maxCoord;

        if (ax > ay) {
            if (ax > az) {
                maxCoord = 0;
            } else {
                maxCoord = 2;
            }
        } else {
            if (ay > az) {
                maxCoord = 1;
            } else {
                maxCoord = 2;
            }
        }

        // solve 2D problem
        switch (maxCoord) {
            case 0: {
                if (1.0 + ax != 1.0) {
                    result[1] = 1.0 + (pu[1] * wu[2] - pu[2] * wu[1]) / nx - 1.0;
                    result[2] = 1.0 + (vu[1] * pu[2] - vu[2] * pu[1]) / nx - 1.0;
                    result[0] = 1.0 - result[1] - result[2];
                }
                break;
            }

            case 1: {
                if (1.0 + ay != 1.0) {
                    result[1] = 1.0 + (pu[2] * wu[0] - pu[0] * wu[2]) / ny - 1.0;
                    result[2] = 1.0 + (vu[2] * pu[0] - vu[0] * pu[2]) / ny - 1.0;
                    result[0] = 1.0 - result[1] - result[2];
                }
                break;
            }

            case 2: {
                if (1.0 + az != 1.0) {
                    result[1] = 1.0 + (pu[0] * wu[1] - pu[1] * wu[0]) / nz - 1.0;
                    result[2] = 1.0 + (vu[0] * pu[1] - vu[1] * pu[0]) / nz - 1.0;
                    result[0] = 1.0 - result[1] - result[2];
                }
                break;
            }
        }

        return result.cast<Real>();
    }

    Vector<Real, 3> FromBarycentric(const Vector<Real, 3> &barycentric, const Vector<Real, 3> &u,
                                    const Vector<Real, 3> &v,
                                    const Vector<Real, 3> &w) {
        return (barycentric[0] * u.cast<double>() +
                barycentric[1] * v.cast<double>() +
                barycentric[2] * w.cast<double>()).cast<Real>();
    }
}
