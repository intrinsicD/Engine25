//
// Created by dieckman on 28.03.25.
//

#include <cuda_runtime.h>
#include <iostream>
#include "CudaUtils.h"
#include "Logger.h"

namespace Bcg::CudaUtils {
    CudaDevices CheckCuda() {
        CudaDevices cuda;
        int device_count = 0;
        cudaError_t result = cudaGetDeviceCount(&device_count);
        if (result != cudaSuccess || device_count == 0) {
            std::cerr << "No CUDA device found: " << cudaGetErrorString(result) << std::endl;
            return {};
        }

        for (int i = 0; i < device_count; i++) {
            cudaDeviceProp device_prop{};
            cudaGetDeviceProperties(&device_prop, i);
            CudaDeviceInfo device_info{};
            device_info.device_count_id = i;
            device_info.device_name = device_prop.name;
            device_info.memory_mb = device_prop.totalGlobalMem / 1024 / 1024;
            device_info.compute_minor = device_prop.minor;
            device_info.compute_major = device_prop.major;
            cuda.devices.push_back(device_info);

            LOG_INFO(fmt::format("CUDA: Device {} {}", i, device_prop.name));
            LOG_INFO(fmt::format("CUDA: Device memory {}MB", device_prop.totalGlobalMem / 1024 / 1024));
            LOG_INFO(fmt::format("CUDA: Compute Capability {}.{}", device_prop.major, device_prop.minor));
        }
        return cuda;
    }
}