#include "Engine.h"
#include <cuda_runtime.h>
#include <iostream>

int main() {

    int deviceCount = 0;
    cudaGetDeviceCount(&deviceCount);
    for (int dev = 0; dev < deviceCount; ++dev) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);
        std::cout << "Device " << dev << ": " << deviceProp.name << "\n";
        std::cout << "  Compute capability: "
                  << deviceProp.major << "." << deviceProp.minor << "\n";
    }

    Bcg::Engine engine;
    engine.init();
    engine.startup();
    engine.run();
    engine.shutdown();

    return 0;
}
