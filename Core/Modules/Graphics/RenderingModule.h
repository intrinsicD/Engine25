//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_RENDERINGMODULE_H
#define ENGINE25_RENDERINGMODULE_H

#include "Module.h"
#include "CommandBuffer.h"

namespace Bcg::Graphics {
    class RenderingModule : public Module {
    public:
        RenderingModule();

        ~RenderingModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        struct Backend {
            enum class Type {
                Unknown,
                OpenGL,
                Vulkan,
            } type;

            std::string name;
            std::string version;

            operator bool() const { return type != Type::Unknown; }
        };

        Backend &GetBackend() { return m_backend; }

        void SetBackend(const Backend &backend) { m_backend = backend; }

        CommandBuffer prepare_render;
        CommandBuffer end_render;

    private:
        Backend m_backend;
    };

    struct InitializeOpenGL : public AbstractCommand {
        InitializeOpenGL() : AbstractCommand() {
        }

        void Execute() const override;
    };

    struct LogThisFrame : public AbstractCommand {
        LogThisFrame() : AbstractCommand() {
        }

        void Execute() const override;
    };

    struct SetClearColor : public AbstractCommand {
        SetClearColor(float clear_color[3]) : AbstractCommand(), color{clear_color[0], clear_color[1], clear_color[2]} {
        }

        SetClearColor(float r, float g, float b) : AbstractCommand(), color{r, g, b} {
        }

        void Execute() const override;

        float color[3];
    };

    struct SetViewport : public AbstractCommand {
        SetViewport(int x, int y, int width, int height) : AbstractCommand(), viewport{x, y, width, height} {
        }

        void Execute() const override;

        int viewport[4];
    };

    struct ClearFrameBuffer : public AbstractCommand {
        explicit ClearFrameBuffer(int component_mask) : AbstractCommand(), component_mask(component_mask) {
        }

        void Execute() const override;

        int component_mask;
    };

    struct ClearFrameBufferColor : public AbstractCommand {
        ClearFrameBufferColor() : AbstractCommand() {
        }

        void Execute() const override;
    };

    struct ClearFrameBufferColorDepth : public AbstractCommand {
        ClearFrameBufferColorDepth() : AbstractCommand() {
        }

        void Execute() const override;
    };
}

#endif //ENGINE25_RENDERINGMODULE_H
