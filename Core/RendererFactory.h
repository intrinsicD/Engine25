//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_RENDERERFACTORY_H
#define ENGINE25_RENDERERFACTORY_H

#include "IRenderer.h"

namespace Bcg{
    namespace RendererFactory {
        std::unique_ptr<IRenderer> CreateRenderer(BackendDesc &backendDesc);
    }
}

#endif //ENGINE25_RENDERERFACTORY_H
