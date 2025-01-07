//
// Created by alex on 1/7/25.
//

#ifndef SPHERE_H
#define SPHERE_H


#include "Math.h"
#include "Mesh.h"

namespace Bcg {
    // Sphere class template for a sphere in N-dimensional space
    template<typename T, int N>
    CUDA_HOST_DEVICE
    class Sphere {
    public:
        // Default constructor - initializes with zero radius and center at origin
        Sphere() : m_center(Vector<T, N>::Zero()), m_radius(0) {
        }

        // Constructor to initialize sphere with a given center and zero radius
        explicit Sphere(const Vector<T, N> &center) : m_center(center), m_radius(0) {
        }

        // Constructor to initialize sphere with a given center and radius
        Sphere(const Vector<T, N> &center, T radius) : m_center(center), m_radius(radius) {
        }

        // Constructor to create a sphere that bounds a set of points
        // The sphere center is the mean of the points, and radius is the maximum distance from the center
        template<typename Iterator>
        Sphere(Iterator first, Iterator last) {
            if (first == last) {
                m_center = Vector<T, N>::Zero();
                m_radius = 0;
                return;
            }
            Vector<T, N> mean = Vector<T, N>::Zero();
            for (auto it = first; it != last; ++it) {
                mean += *it;
            }
            mean /= std::distance(first, last);
            m_center = mean;
            T max_distance = 0;
            for (auto it = first; it != last; ++it) {
                max_distance = std::max(max_distance, (mean - *it).norm());
            }
            m_radius = max_distance;
        }

        // Expands the sphere to include a given point if it lies outside the current sphere
        void grow(const Vector<T, N> &point) {
            T distance = (point - m_center).norm();
            if (distance > m_radius) {
                m_radius = distance;
            }
        }

        // Returns the center of the sphere
        [[nodiscard]] const Vector<T, N> &center() const {
            return m_center;
        }

        // Returns the radius of the sphere
        [[nodiscard]] T radius() const {
            return m_radius;
        }

        // Computes the volume of the sphere
        [[nodiscard]] T volume() const {
            return std::pow(std::numbers::pi_v<T>, static_cast<T>(N) / 2) * std::pow(m_radius, N) /
                   std::tgamma(static_cast<T>(N) / 2 + 1);
        }

        // Computes the surface area of the sphere
        [[nodiscard]] T surface_area() const {
            return 2 * std::pow(std::numbers::pi_v<T>, static_cast<T>(N) / 2) * std::pow(m_radius, N - 1) /
                   std::tgamma(static_cast<T>(N) / 2);
        }

    private:
        Vector<T, N> m_center; // Center of the sphere
        T m_radius; // Radius of the sphere
    };
}

#endif //SPHERE_H