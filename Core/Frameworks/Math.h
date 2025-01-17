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

    template<typename T, int N>
    CUDA_HOST_DEVICE
    Vector<T, N> CumulativeProduct(const Vector<T, N> &input) {
        Vector<T, N> output = Vector<T, N>::Ones();
        if (input.size() > 0) {
            output[0] = input[0];
            for (int i = 1; i < input.size(); ++i) {
                output[i] = output[i - 1] * input[i];
            }
        }
        return output;
    }
}

#endif //MATH_H
