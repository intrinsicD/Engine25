// AABB.h
// Created by alex on 1/7/25

#ifndef AABB_H
#define AABB_H

#include "Math.h"
#include "Macros.h"

namespace Bcg {
    // Template class representing an Axis-Aligned Bounding Box (AABB) in N-dimensional space
    template<typename T, int N>
    CUDA_HOST_DEVICE
    class AABB {
    public:
        // Default constructor initializes the AABB to an invalid state with extreme min and max bounds
        AABB() : m_min(Vector<T, N>::Constant(std::numeric_limits<T>::max())),
                 m_max(Vector<T, N>::Constant(std::numeric_limits<T>::lowest())) {
        }

        // Constructor initializing the AABB to enclose a single point
        explicit AABB(const Vector<T, N> &point) : m_min(point), m_max(point) {
        }

        // Constructor initializing the AABB with specified minimum and maximum bounds
        AABB(const Vector<T, N> &min, const Vector<T, N> &max) : m_min(min), m_max(max) {
        }

        // Constructor to create an AABB enclosing a range of points specified by iterators
        template<typename Iterator>
        AABB(Iterator first, Iterator last) {
            for (auto it = first; it != last; ++it) {
                grow(*it);
            }
        }

        // Expands the AABB to include the given point
        void grow(const Vector<T, N> &point) {
            m_min = m_min.cwiseMin(point);
            m_max = m_max.cwiseMax(point);
        }

        // Returns the minimum corner of the AABB
        [[nodiscard]] const Vector<T, N> &min() const {
            return m_min;
        }

        // Returns the maximum corner of the AABB
        [[nodiscard]] const Vector<T, N> &max() const {
            return m_max;
        }

        // Computes and returns the center of the AABB
        [[nodiscard]] Vector<T, N> center() const {
            return (m_min + m_max) / 2;
        }

        // Computes and returns the extents (diagonal lengths) of the AABB
        [[nodiscard]] Vector<T, N> extents() const {
            return m_max - m_min;
        }

        // Computes and returns the volume of the AABB
        [[nodiscard]] T volume() const {
            return product(extents());
        }

        // Computes and returns the surface area of the AABB
        [[nodiscard]] T surface_area() const {
            Vector<T, N> diag = m_max - m_min;
            T total = 0;

            // Precompute cumulative products from left and right
            Vector<T, N> left(N, 1), right(N, 1);
            for (int i = 1; i < N; ++i) {
                left[i] = left[i - 1] * diag[i - 1];
            }
            for (int i = N - 2; i >= 0; --i) {
                right[i] = right[i + 1] * diag[i + 1];
            }

            // Compute surface area
            for (int i = 0; i < N; ++i) {
                total += left[i] * right[i];
            }

            return 2 * total;
        }

        // Returns the index of the dimension with the largest extent
        [[nodiscard]] int max_extent() const {
            Vector<T, N> diag = m_max - m_min;
            return diag.max_dimension();
        }

        [[nodiscard]] int size() const {
            return m_min.size();
        }

    private:
        Vector<T, N> m_min; // Minimum corner of the AABB
        Vector<T, N> m_max; // Maximum corner of the AABB
    };
}

#endif //AABB_H
