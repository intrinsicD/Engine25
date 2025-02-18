//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_WINDOWCOMPONENT_H
#define ENGINE25_WINDOWCOMPONENT_H

#include "PoolHandle.h"
#include "Pool.h"
#include "Frameworks/Graphics/GlfwWindow.h"

namespace Bcg::Graphics{
    using WindowComponent = PoolHandle<Window>;
}

#endif //ENGINE25_WINDOWCOMPONENT_H
