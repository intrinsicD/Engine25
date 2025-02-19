//
// Created by alex on 05.12.24.
//

#include "GuiModule.h"
#include "GLFWModule.h"
#include "RenderingModule.h"
#include "WindowComponent.h"
#include "Engine.h"
#include "MainLoop.h"
#include "ConfigFile.h"
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "Cache.h"
#include <filesystem>

#include "Math.h"

namespace Bcg::Graphics {
    namespace fs = std::filesystem;

    GuiModule::GuiModule() : Module("GuiModule", "0.1") {
    }

    void GuiModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&GuiModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Startup>().connect<&GuiModule::on_startup>(this);
        Engine::get_dispatcher().sink<Events::Synchronize>().connect<&GuiModule::on_synchronize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&GuiModule::on_shutdown>(this);
        Module::connect_events();
    }

    void GuiModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&GuiModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Startup>().disconnect<&GuiModule::on_startup>(this);
        Engine::get_dispatcher().sink<Events::Synchronize>().disconnect<&GuiModule::on_synchronize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&GuiModule::on_shutdown>(this);
        Module::disconnect_events();
    }

    void GuiModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<InitializeImGui>());
    }

    void GuiModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void GuiModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.end_render.Next().AddCommand(std::make_shared<BeginGui>());
        loop.render_gui.Next().AddCommand(std::make_shared<MockMenu>());
        loop.render_gui.Next().AddCommand(std::make_shared<RenderMenu>());
        loop.render_gui.Next().AddCommand(std::make_shared<RenderGui>());
        loop.end_gui.Next().AddCommand(std::make_shared<EndGui>());
    }

    void GuiModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }

    bool GuiModule::WantCaptureKeyboard() {
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool GuiModule::WantCaptureMouse() {
        return ImGui::GetIO().WantCaptureMouse;
    }

    void GuiModule::LoadFonts(const std::string &font_path, float font_size) {
        auto &io = ImGui::GetIO();
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
        io.Fonts->Build();
        LOG_INFO(fmt::format("GuiModule::LoadFonts: Loaded: {} font_size {}", font_path, font_size));
    }


    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void InitializeImGui::Execute() const {
        if (!Engine::get_context().contains<ImGuiContext>()) {
            IMGUI_CHECKVERSION();
            auto &context = Engine::get_context().emplace<ImGuiContext *>(ImGui::CreateContext());

            auto &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.IniFilename = nullptr;

            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
            ImGui_ImplOpenGL3_Init();

            float dpi = GLFWModule::GetDpiScaling();
            std::string fontPath = Config::get_string("font.path");
            std::string defaultFont = Config::get_string("font.default_font");
            float fontSize = Config::get_float("font.size");
            fs::path path = fs::path(SOURCE_DIR_PATH) / fontPath / defaultFont;
            LOG_INFO(fmt::format("Command::InitiallizeImGui: DPI Scaling: {}", dpi));
            LOG_INFO(fmt::format("Command::InitiallizeImGui: Font size: {}", fontSize));
            LOG_INFO(fmt::format("Command::InitiallizeImGui: Loaded: {}", path.c_str()));

            GuiModule::LoadFonts(path, fontSize * dpi);
            ImGui::GetStyle().ScaleAllSizes(dpi);
        } else {
            LOG_WARN(fmt::format("Command::InitiallizeImGui: ImGui Context already initialized"));
        }
    }

    void BeginGui::Execute() const {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGuizmo::BeginFrame();

        ImGui::BeginMainMenuBar();
        LOG_FRAME("Command::BeginGui");
    }

    void RenderMenu::Execute() const {
        Engine::get_dispatcher().trigger<Events::Gui::RenderMenu>();
        LOG_FRAME("Command::RenderMenu: trigger event");
    }

    void RenderGui::Execute() const {
        Engine::get_dispatcher().trigger<Events::Gui::RenderGui>();
        LOG_FRAME("Command::RenderGui: trigger event");
    }

    void MockMenu::Execute() const {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                LOG_INFO("New");
            }
            if (ImGui::MenuItem("Open")) {
                LOG_INFO("Open");
            }
            if (ImGui::MenuItem("Save")) {
                LOG_INFO("Save");
            }
            if (ImGui::MenuItem("Exit")) {
                LOG_INFO("Exit");
            }
            ImGui::EndMenu();
        }
        auto &window = Engine::get_context().get<WindowComponent>();
        static bool show_window_gui = true;
        if (ImGui::Begin("Window", &show_window_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::ColorEdit3("clear_color", window->clear_color.data())) {
                SetClearColor(window->clear_color.data()).Execute();
            }
            ImGui::Text("Width %d", window->width);
            ImGui::Text("Height %d", window->height);
        }
        ImGui::End();
        LOG_FRAME("Command::MockMenu");
    }

    void EndGui::Execute() const {
        ImGui::EndMainMenuBar();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        auto &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        LOG_FRAME("Command::EndGui");
    }
}
