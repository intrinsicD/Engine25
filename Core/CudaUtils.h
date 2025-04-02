q//
// Created by dieckman on 27.03.25.
//

#ifndef CUDAUTILS_H
#define CUDAUTILS_H

#include <vector>

namespace Bcg::CudaUtils {
    struct CudaDeviceInfo {
        std::string device_name;
        int device_count_id;
        size_t memory_mb;
        int compute_minor;
        int compute_major;
    };

    struct CudaDevices {
        std::vector<CudaDeviceInfo> devices;

        operator bool() const { return !devices.empty(); }
    };

    CudaDevices CheckCuda();
}

#endif //CUDAUTILS_H
