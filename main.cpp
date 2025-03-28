#include "Engine.h"
#include "CudaUtils.h"

int main() {
    Bcg::CudaUtils::CheckCuda();

    Bcg::Engine engine;
    engine.init();
    engine.startup();
    engine.run();
    engine.shutdown();

    return 0;
}
