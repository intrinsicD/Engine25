#include "Engine.h"

int main() {

    Bcg::Engine engine;
    engine.init();
    engine.startup();
    engine.run();
    engine.shutdown();

    return 0;
}
