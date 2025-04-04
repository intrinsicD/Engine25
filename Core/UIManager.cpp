//
// Created by alex on 02.04.25.
//

#include "UIManager.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "ConfigFile.h"
#include "Events/EngineEvents.h"
#include "entt/signal/dispatcher.hpp"
#include <filesystem>

namespace Bcg {
    namespace fs = std::filesystem;

    UIManager::UIManager() : IModule("ImGui Module", "0.1.0") {}

    bool UIManager::initialize(Bcg::ApplicationContext *context) {
        if (!context) {
            LOG_FATAL("UIManager: Context is null");
            return false;
        }

        if (isInitialized()) {
            LOG_WARN("UIManager: Already initialized");
            return false;
        }

        if(setContext(context)) {
            if (!context->windowPtr) {
                LOG_ERROR("UIManager: Context window is null");
                return false;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            auto &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.IniFilename = nullptr;

            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(context->windowPtr, true);
            ImGui_ImplOpenGL3_Init();

            std::string fontPath = Config::get_string("font.path");
            std::string defaultFont = Config::get_string("font.default_font");
            float fontSize = Config::get_float("font.size");
            fs::path path = fs::path(SOURCE_DIR_PATH) / fontPath / defaultFont;

            auto dpi = getDpi();
            if (loadFont(path, fontSize * dpi)) {
                ImGui::GetStyle().ScaleAllSizes(dpi);
            }
        }else{
            LOG_ERROR("UIManager: Context is null");
            return false;
        }

        LOG_INFO("UIManager: Initialized");
        return true;
    }

    void UIManager::shutdown() {
        if (!isInitialized()) {
            return;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        LOG_INFO(fmt::format("UIManager: Shutdown"));
        m_app_context = nullptr;
    }

    void UIManager::beginFrame() {
        if (!isInitialized()) return;
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Enable docking over the main viewport
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::BeginMainMenuBar();
        LOG_FRAME("UIManager: beginFrame");
    }

    void UIManager::endFrame() {
        if (!isInitialized()) return;
        // Rendering ImGui
        ImGui::EndMainMenuBar();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        LOG_FRAME("UIManager: endFrame");
    }

    // Add methods later to register UI rendering callbacks from modules
    void UIManager::renderMainMenuBar(entt::dispatcher &dispatcher) {
        if (!isInitialized()) return;
        dispatcher.trigger<Events::Gui::RenderMenu>();

        if (m_demo_menu) {
            if (ImGui::BeginMenu("Demo")) {
                ImGui::MenuItem("Show Demo", nullptr, &m_demo_window);
                ImGui::EndMenu();
            }
        }
    }

    void UIManager::renderModuleUI(entt::dispatcher &dispatcher) {
        if (!isInitialized()) return;
        dispatcher.trigger<Events::Gui::RenderGui>();

        if (m_demo_window) {
            ImGui::ShowDemoWindow(&m_demo_window);
        }
    }

    inline float ComputeDpi(int width_pixels, int height_pixels, float width_mm, float height_mm) {
        // Convert millimeters to inches (1 inch = 25.4 mm)
        float dpi_x = (width_pixels * 25.4f) / width_mm;
        float dpi_y = (height_pixels * 25.4f) / height_mm;
        // Average the horizontal and vertical DPI
        return (dpi_x + dpi_y) * 0.5f;
    }

    float UIManager::getDpi() {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (!monitor) {
            return 1.0f;
        }
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (!mode) {
            return 1.0f;
        }
        int width_mm = 0, height_mm = 0;
        glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
        if (width_mm == 0 || height_mm == 0) {
            return 1.0f;
        }

        float dpi = ComputeDpi(mode->width, mode->height, static_cast<float>(width_mm), static_cast<float>(height_mm));
        return dpi / 96.0f;
    }

    bool UIManager::loadFont(const std::string &font_path, float font_size) {
        if (!fs::exists(font_path)) {
            LOG_ERROR(fmt::format("UIManager: Font file {} does not exist", font_path));
            return false;
        }
        auto &io = ImGui::GetIO();
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
        io.Fonts->Build();
        LOG_INFO(fmt::format("UIManager: Loaded font {} with size {}", font_path, font_size));
        return true;
    }
}