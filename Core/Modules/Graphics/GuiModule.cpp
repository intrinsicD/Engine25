//
// Created by alex on 05.12.24.
//

#include "GuiModule.h"
#include "GLFWModule.h"
#include "WindowComponent.h"
#include "Engine.h"
#include "ConfigFile.h"
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

namespace Bcg {
    GuiModule::GuiModule() : Module("GuiModule", "0.1") {
    }

    void GuiModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);
    }

    void GuiModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void GuiModule::OnSynchronize(const Events::Synchronize &event) {
    }

    void GuiModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
    }

    void GuiModule::LoadFonts(std::string font_path, float font_size) {
        auto &io = ImGui::GetIO();
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
        io.Fonts->Build();
    }


    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void InitializeImGui::Execute() const {
        if (!Engine::GetContext().contains<ImGuiContext>()) {
            IMGUI_CHECKVERSION();
            auto &context = Engine::GetContext().emplace<ImGuiContext*>(ImGui::CreateContext());

            auto &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.IniFilename = nullptr;

            ImGui::StyleColorsDark();
            auto &h_window = Engine::GetContext().get<WindowComponent>();
            if (h_window->handle == nullptr) {
                LOG_ERROR(fmt::format("Command::InitiallizeImGui: GLFW window handle is null"));
            }
            ImGui_ImplGlfw_InitForOpenGL(h_window->handle, true);
            ImGui_ImplOpenGL3_Init();

            float dpi = GLFWModule::GetDpiScaling();
            std::string fontPath = Config::GetString("font.path");
            std::string defaultFont = Config::GetString("font.default_font");
            float fontSize = Config::GetFloat("font.size");
            //float fontSize = 16.0f;
            GuiModule::LoadFonts(std::string(SOURCE_DIR_PATH) + "/" + fontPath + "/" + defaultFont, fontSize * dpi);
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
    }

    void EndGui::Execute() const {
        ImGui::Render();
        auto &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

}
