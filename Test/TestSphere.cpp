//
// Created by alex on 1/7/25.
//

#include "SphereUtils.h"
#include <gtest/gtest.h>
#include <vector>

using namespace Bcg;

TEST(SphereTests, DefaultConstructor) {
    Sphere<float, 3> sphere;
    EXPECT_EQ(sphere.radius(), 0);
    EXPECT_EQ(sphere.center(), (Vector<float, 3>::Zero()));
}

TEST(SphereTests, ConstructorWithCenter) {
    Vector<float, 3> center(1.0f, 2.0f, 3.0f);
    Sphere<float, 3> sphere(center);
    EXPECT_EQ(sphere.radius(), 0);
    EXPECT_EQ(sphere.center(), center);
}

TEST(SphereTests, ConstructorWithCenterAndRadius) {
    Vector<float, 3> center(1.0f, 2.0f, 3.0f);
    float radius = 5.0f;
    Sphere<float, 3> sphere(center, radius);
    EXPECT_EQ(sphere.radius(), radius);
    EXPECT_EQ(sphere.center(), center);
}

TEST(SphereTests, ConstructorWithPoints) {
    std::vector<Vector<float, 3>> points = {
        {1.0f, 1.0f, 1.0f},
        {2.0f, 2.0f, 2.0f},
        {3.0f, 3.0f, 3.0f}
    };
    Sphere<float, 3> sphere(points.begin(), points.end());
    EXPECT_NEAR(sphere.center()[0], 2.0f, 1e-5);
    EXPECT_NEAR(sphere.center()[1], 2.0f, 1e-5);
    EXPECT_NEAR(sphere.center()[2], 2.0f, 1e-5);
    EXPECT_NEAR(sphere.radius(), std::sqrt(3.0f), 1e-5);
}

TEST(SphereTests, Grow) {
    Sphere<float, 3> sphere(Vector<float, 3>::Zero(), 1.0f);
    Vector<float, 3> point(3.0f, 0.0f, 0.0f);
    sphere.grow(point);
    EXPECT_NEAR(sphere.radius(), 3.0f, 1e-5);
}

TEST(SphereTests, Merge) {
    Sphere<float, 3> sphere1(Vector<float, 3>(0.0f, 0.0f, 0.0f), 1.0f);
    Sphere<float, 3> sphere2(Vector<float, 3>(4.0f, 0.0f, 0.0f), 2.0f);
    Sphere<float, 3> merged = Merge(sphere1, sphere2);
    EXPECT_NEAR(merged.radius(), 3.5f, 1e-5);
    EXPECT_NEAR(merged.center()[0], 2.5f, 1e-5);
    EXPECT_NEAR(merged.center()[1], 0.0f, 1e-5);
    EXPECT_NEAR(merged.center()[2], 0.0f, 1e-5);
}

TEST(SphereTests, Intersects) {
    Sphere<float, 3> sphere1(Vector<float, 3>(0.0f, 0.0f, 0.0f), 2.0f);
    Sphere<float, 3> sphere2(Vector<float, 3>(3.0f, 0.0f, 0.0f), 2.0f);
    EXPECT_TRUE(Intersects(sphere1, sphere2));
    Sphere<float, 3> sphere3(Vector<float, 3>(5.0f, 0.0f, 0.0f), 2.0f);
    EXPECT_FALSE(Intersects(sphere1, sphere3));
}

TEST(SphereTests, Contains) {
    Sphere<float, 3> sphere1(Vector<float, 3>(0.0f, 0.0f, 0.0f), 3.0f);
    Sphere<float, 3> sphere2(Vector<float, 3>(1.0f, 1.0f, 1.0f), 1.0f);
    EXPECT_TRUE(Contains(sphere1, sphere2));
    Sphere<float, 3> sphere3(Vector<float, 3>(2.0f, 2.0f, 2.0f), 2.0f);
    EXPECT_FALSE(Contains(sphere1, sphere3));
}

TEST(SphereTests, ClosestPoint) {
    Sphere<float, 3> sphere(Vector<float, 3>::Zero(), 2.0f);
    Vector<float, 3> point(3.0f, 0.0f, 0.0f);
    Vector<float, 3> closest = ClosestPoint(sphere, point);
    EXPECT_NEAR(closest[0], 2.0f, 1e-5);
    EXPECT_NEAR(closest[1], 0.0f, 1e-5);
    EXPECT_NEAR(closest[2], 0.0f, 1e-5);
}

TEST(SphereTests, MinDist) {
    Sphere<float, 3> sphere(Vector<float, 3>::Zero(), 2.0f);
    Vector<float, 3> point(5.0f, 0.0f, 0.0f);
    EXPECT_NEAR(MinSqDist(sphere, point), 9.0f, 1e-5);
}

TEST(SphereTests, MinMaxSqDist) {
    Sphere<float, 3> sphere(Vector<float, 3>::Zero(), 2.0f);
    Vector<float, 3> point(5.0f, 0.0f, 0.0f);
    float min_dist = MinSqDist(sphere, point);
    float min_max_dist = MinMaxSqDist(sphere, point);
    EXPECT_LE(min_dist, min_max_dist);
    EXPECT_NEAR(min_max_dist, 9.0f, 1e-5);
}
