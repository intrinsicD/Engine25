//
// Created by alex on 1/7/25.
//

#include "AABBUtils.h"
#include <gtest/gtest.h>

namespace Bcg {
    TEST(AABBTest, DefaultConstructor) {
        AABB<float, 3> aabb;
        EXPECT_EQ(aabb.min(), (Vector<float, 3>::Constant(std::numeric_limits<float>::max())));
        EXPECT_EQ(aabb.max(), (Vector<float, 3>::Constant(std::numeric_limits<float>::lowest())));
    }

    TEST(AABBTest, PointConstructor) {
        Vector<float, 3> point(1.0f, 2.0f, 3.0f);
        AABB<float, 3> aabb(point);
        EXPECT_EQ(aabb.min(), point);
        EXPECT_EQ(aabb.max(), point);
    }

    TEST(AABBTest, MinMaxConstructor) {
        Vector<float, 3> min(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb(min, max);
        EXPECT_EQ(aabb.min(), min);
        EXPECT_EQ(aabb.max(), max);
    }

    TEST(AABBTest, Grow) {
        Vector<float, 3> min(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb(min, max);

        Vector<float, 3> point(0.0f, 6.0f, 2.0f);
        aabb.grow(point);

        EXPECT_EQ(aabb.min(), (Vector<float, 3>(0.0f, 2.0f, 2.0f)));
        EXPECT_EQ(aabb.max(), (Vector<float, 3>(4.0f, 6.0f, 6.0f)));
    }

    TEST(AABBTest, Merge) {
        Vector<float, 3> min1(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max1(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb1(min1, max1);

        Vector<float, 3> min2(0.0f, 3.0f, 2.0f);
        Vector<float, 3> max2(5.0f, 6.0f, 7.0f);
        AABB<float, 3> aabb2(min2, max2);

        AABB<float, 3> merged = Merge(aabb1, aabb2);

        EXPECT_EQ(merged.min(), (Vector<float, 3>(0.0f, 2.0f, 2.0f)));
        EXPECT_EQ(merged.max(), (Vector<float, 3>(5.0f, 6.0f, 7.0f)));
    }

    TEST(AABBTest, Intersects) {
        Vector<float, 3> min1(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max1(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb1(min1, max1);

        Vector<float, 3> min2(3.0f, 4.0f, 5.0f);
        Vector<float, 3> max2(6.0f, 7.0f, 8.0f);
        AABB<float, 3> aabb2(min2, max2);

        EXPECT_TRUE(Intersects(aabb1, aabb2));

        Vector<float, 3> min3(5.0f, 6.0f, 7.0f);
        Vector<float, 3> max3(8.0f, 9.0f, 10.0f);
        AABB<float, 3> aabb3(min3, max3);

        EXPECT_FALSE(Intersects(aabb1, aabb3));
    }

    TEST(AABBTest, ClosestPoint) {
        Vector<float, 3> min(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb(min, max);

        Vector<float, 3> point(0.0f, 3.0f, 7.0f);
        Vector<float, 3> closest = ClosestPoint(aabb, point);

        EXPECT_EQ(closest, (Vector<float, 3>(1.0f, 3.0f, 6.0f)));
    }

    TEST(AABBTest, MinDist) {
        Vector<float, 3> min(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb(min, max);

        Vector<float, 3> point(0.0f, 3.0f, 7.0f);
        float min_dist = MinSqDist(aabb, point);

        EXPECT_FLOAT_EQ(min_dist, 2.0f); // sqrt(1^2 + 0^2 + 1^2)
    }

    template<typename T>
    CUDA_HOST_DEVICE
    T MinMaxDistRef3D(const AABB<T, 3> &lhs, const Vector<T, 3> &rhs) {
        Vector<T, 3> lower_diff = lhs.min() - rhs;
        Vector<T, 3> upper_diff = lhs.max() - rhs;
        Vector<T, 3> rm_sq(lower_diff.x() * lower_diff.x(), lower_diff.y() * lower_diff.y(),
                           lower_diff.z() * lower_diff.z());
        Vector<T, 3> rM_sq(upper_diff.x() * upper_diff.x(), upper_diff.y() * upper_diff.y(),
                           upper_diff.z() * upper_diff.z());

        if ((lhs.max().x() + lhs.min().x()) * 0.5f < rhs.x()) {
            Swap(rm_sq.x(), rM_sq.x());
        }
        if ((lhs.max().y() + lhs.min().y()) * 0.5f < rhs.y()) {
            Swap(rm_sq.y(), rM_sq.y());
        }
        if ((lhs.max().z() + lhs.min().z()) * 0.5f < rhs.z()) {
            Swap(rm_sq.z(), rM_sq.z());
        }

        const T dx = rm_sq.x() + rM_sq.y() + rM_sq.z();
        const T dy = rM_sq.x() + rm_sq.y() + rM_sq.z();
        const T dz = rM_sq.x() + rM_sq.y() + rm_sq.z();
        return ::fminf(dx, ::fminf(dy, dz));
    }

    TEST(AABBTest, MinMaxSqDist) {
        Vector<float, 3> min(1.0f, 2.0f, 3.0f);
        Vector<float, 3> max(4.0f, 5.0f, 6.0f);
        AABB<float, 3> aabb(min, max);

        Vector<float, 3> point(0.0f, 3.0f, 7.0f);
        float min_max_dist = MinMaxSqDist(aabb, point);
        float min_max_distRef3D = MinMaxDistRef3D(aabb, point);
        float min_dist = MinSqDist(aabb, point);

        EXPECT_LE(min_dist, min_max_dist); // Minimum distance is always less than minimum maximum distance
        EXPECT_FLOAT_EQ(min_max_dist, 21.0f); // Minimum max distance calculation
        EXPECT_FLOAT_EQ(min_max_dist, min_max_distRef3D); // Minimum max distance calculation
    }
}
