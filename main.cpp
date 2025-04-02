#include "Engine.h"
#include "CudaUtils.h"
#include "Application.h"

int main() {
    Bcg::CudaUtils::CheckCuda();

/*    Bcg::Engine engine;
    engine.init();
    engine.startup();
    engine.run();
    engine.shutdown();*/

    Bcg::Application app(1280, 720, "My Application");
    app.initialize();
    app.run();

    return 0;
}
