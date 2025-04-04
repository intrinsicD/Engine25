//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_OPENGLRENDERER_H
#define ENGINE25_OPENGLRENDERER_H

#include "IRenderer.h"

namespace Bcg{
    class OpenglRenderer : public IRenderer{
    public:
        OpenglRenderer();

        ~OpenglRenderer() override = default;

        bool initialize(ApplicationContext *context) override;

        void shutdown() override;

        void render() override;

        void beginFrame() override;

        void endFrame() override;

    private:
        void update(float delta_time) override{

        }

        void renderUI() override{

        }

        void connectDispatcher() override{

        }

        void disconnectDispatcher() override{

        }
    };
}

#endif //ENGINE25_OPENGLRENDERER_H
