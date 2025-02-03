//
// Created by alex on 1/7/25.
//

#ifndef ENGINE25_MACROS_H
#define ENGINE25_MACROS_H

namespace Bcg {
#ifdef __CUDACC__
#define CUDA_HOST_DEVICE __host__ __device__
#define CUDA_DEVICE __device__
#define CUDA_HOST __host__
#else
#define CUDA_HOST_DEVICE
#define CUDA_DEVICE
#define CUDA_HOST
#endif
}

#endif //ENGINE25_MACROS_H
