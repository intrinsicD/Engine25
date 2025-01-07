//
// Created by alex on 1/7/25.
//

#ifndef MATH_H
#define MATH_H

#include "Eigen/Core"
#include "Macros.h"
#include <numbers>

namespace Bcg {
    using Real = float;

    template<typename T, int N>
    using Vector = Eigen::Vector<T, N>;

    template<typename T, int Rows, int Cols>
    using Matrix = Eigen::Matrix<T, Rows, Cols>;


    template<typename T1, typename T2>
    CUDA_HOST_DEVICE
    void Swap(T1 &a, T2 &b) {
        auto tmp = a;
        a = b;
        b = tmp;
    }
}

#endif //MATH_H
