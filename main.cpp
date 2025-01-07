#include "Engine.h"

int main() {

    Bcg::Engine engine;
    engine.Init();
    engine.Startup();
    engine.Run();
    engine.Shutdown();

    return 0;
}
