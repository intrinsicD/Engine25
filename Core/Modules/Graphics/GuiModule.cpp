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

namespace Bcg {
    namespace fs = std::filesystem;

    GuiModule::GuiModule() : Module("GuiModule", "0.1") {
    }

    void GuiModule::ConnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&GuiModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&GuiModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&GuiModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&GuiModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void GuiModule::DisconnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&GuiModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&GuiModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&GuiModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&GuiModule::OnShutdown>(*this);
        Module::DisconnectEvents();
    }

    void GuiModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<InitializeImGui>());
    }

    void GuiModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void GuiModule::OnSynchronize(const Events::Synchronize &event) {
        Module::OnSynchronize(event);
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.end_render.Next().AddCommand(std::make_shared<BeginGui>());
        loop.render_gui.Next().AddCommand(std::make_shared<MockMenu>());
        loop.end_gui.Next().AddCommand(std::make_shared<EndGui>());
    }

    void GuiModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
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
        if (!Engine::GetContext().contains<ImGuiContext>()) {
            IMGUI_CHECKVERSION();
            auto &context = Engine::GetContext().emplace<ImGuiContext *>(ImGui::CreateContext());

            auto &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.IniFilename = nullptr;

            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
            ImGui_ImplOpenGL3_Init();

            float dpi = GLFWModule::GetDpiScaling();
            std::string fontPath = Config::GetString("font.path");
            std::string defaultFont = Config::GetString("font.default_font");
            float fontSize = Config::GetFloat("font.size");
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
        auto &window = Engine::GetContext().get<WindowComponent>();
        static bool show_window_gui = true;
        if (ImGui::Begin("Window", &show_window_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::ColorEdit3("clear_color", window->clear_color)) {
                SetClearColor(window->clear_color).Execute();
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
