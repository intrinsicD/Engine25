//
// Created by alex on 02.04.25.
//

#include "Application.h"
#include "Logger.h"
#include "ConfigFile.h"
#include "Platform.h" // Include Platform
#include "VulkanRenderer.h" // Include specific modules
#include "MeshRenderPass.h" // Include specific modules
#include "UIManager.h"
#include "ModuleManager.h"
#include "ApplicationContext.h" // Ensure context is included
#include "GLFW/glfw3.h" // For glfwGetTime if still used here

#include <stdexcept> // For exception handling

namespace Bcg {

    Application::Application(int width, int height, const char *name) : m_width(width), m_height(height) {
        if (name) {
            m_name = name;
        }

        Logger::get_instance().set_log_file("engine.log");
        Logger::get_instance().enable_console_logger(true);
        LOG_INFO(fmt::format("Application {} v{} starting...", m_name, m_version));

        // Config loading should ideally happen before module creation if modules depend on it
        try {
            Config::load_config(std::string(SOURCE_DIR_PATH) + "/config.json");
        } catch (const std::exception& e) {
            LOG_ERROR(fmt::format("Failed to load config file: {}", e.what()));
            // Decide how to handle config load failure (e.g., use defaults, throw)
            // Using defaults from context initialization below for now.
        }

        // Initialize ApplicationContext members *before* passing it to modules
        m_app_context.scene = &m_scene;
        m_app_context.dispatcher = &m_dispatcher;
        m_app_context.windowDesc.width = Config::get_int("window.width"); // Use defaults
        m_app_context.windowDesc.height = Config::get_int("window.height");
        m_app_context.windowDesc.title = Config::get_string("window.title"); // Use default

        // Backend config - crucial for renderer selection later if needed
        m_app_context.backend.type = Config::get_string("backend.type"); // Default to Vulkan
        // ... (load other backend settings as before) ...
        m_app_context.backend.vsync = Config::get_int("backend.vsync");


        // Create managers and modules AFTER basic context setup
        m_platform = std::make_unique<Platform>();
        m_renderer = std::make_unique<VulkanRenderer>(); // Assuming Vulkan for now
        m_ui_manager = std::make_unique<UIManager>();
        m_module_manager = std::make_unique<ModuleManager>(); // ModuleManager might add more modules
        m_shader_module = std::make_unique<SlangShaderModule>(); // Create instance
        // Register core modules with ModuleManager if desired (optional)
        // m_module_manager->registerModule(m_renderer);
        // m_module_manager->registerModule(m_ui_manager);
    }

    // Need destructor definition
    Application::~Application() {
        LOG_INFO("Application shutting down...");
        // Ensure shutdown is called if not already done in run() exception handling
        try {
            shutdown();
        } catch (const std::exception& e) {
            LOG_ERROR(fmt::format("Exception during application shutdown: {}", e.what()));
        } catch (...) {
            LOG_ERROR("Unknown exception during application shutdown.");
        }
        LOG_INFO("Application destruction complete.");
    }


    void Application::initialize() {
        m_platform->initialize(&m_app_context);
        if (!m_app_context.windowPtr) {
            throw std::runtime_error("Platform failed to create window!");
        }

        // *** Initialize Slang Module ***
        // It doesn't strictly depend on the window, but might need Vulkan context later
        if (!m_shader_module->initialize(&m_app_context)) {
            throw std::runtime_error("Slang Shader Module initialization failed!");
        }
        m_app_context.shaderModule = m_shader_module.get(); // Store pointer in context

        // ... initialize renderer (renderer might grab device handle in its init) ...
        m_app_context.renderer = m_renderer.get();
        if (!m_renderer->initialize(&m_app_context)) { // Renderer uses the window ptr from context
            throw std::runtime_error("Renderer initialization failed!");
        }
        // The shader module should now have access to vkDevice via context->renderer->GetVulkanContext() if needed

        // *** Pass renderer pointer to context AFTER renderer is created ***
        m_app_context.renderer = m_renderer.get();
        m_app_context.uiManager = m_ui_manager.get(); // Pass UI manager pointer too

        // Initialize core systems that depend on the window/platform
        if (!m_renderer->initialize(&m_app_context)) { // Renderer uses the window ptr from context
            throw std::runtime_error("Renderer initialization failed!");
        }

        // ***** ADD RENDER PASS HERE *****
        if (m_renderer->isInitialized()) {
            LOG_INFO("Adding default MeshRenderPass...");
            // Ensure MeshRenderPass.h is included
#include "MeshRenderPass.h" // Or your specific pass header
            auto meshPass = std::make_shared<MeshRenderPass>();
            m_renderer->AddRenderPass(meshPass);
        }
        // ********************************


        if (!m_ui_manager->initialize(&m_app_context)) { // UI Manager might need renderer context
            throw std::runtime_error("UI Manager initialization failed!");
        }

        // Initialize other modules managed by ModuleManager
        m_module_manager->initializeModules(&m_app_context);

        m_lastFrameTime = m_platform->getTime(); // Use platform's time function
        LOG_INFO("Application initialization complete.");
    }

    void Application::run() {
        LOG_INFO("Starting main loop...");
        if (!m_app_context.windowPtr) {
            LOG_ERROR("Cannot run without a valid window handle.");
            return;
        }

        while (!m_platform->shouldClose()) { // Use Platform to check window close
            float currentTime = (float) glfwGetTime(); // Or Platform::getTime()
            float deltaTime = currentTime - m_lastFrameTime;
            m_lastFrameTime = currentTime;

            // Clamp delta time to avoid large jumps
            deltaTime = std::min(deltaTime, 0.1f); // e.g., max 100ms step

            m_platform->pollEvents(); // Input processing

            update(deltaTime);    // Update game logic

            render();             // Render the frame
        }
        LOG_INFO("Main loop finished.");
        // Shutdown is now called implicitly by the destructor,
        // or explicitly if run exits cleanly (though destructor is safer)
    }

    void Application::update(float deltaTime) {
        // Update application-specific logic if any

        // Update engine systems/modules
        m_renderer->update(deltaTime); // Might be minimal for renderer itself
        m_ui_manager->update(deltaTime); // UI might need updates
        m_module_manager->updateModules(deltaTime); // Update custom modules
    }

    void Application::render() {
        // Renderer handles the core drawing process
        m_renderer->render(); // This now internally handles render passes

        // UI Rendering might be a RenderPass itself or handled separately
        // If UIManager uses ImGui, it often needs specific begin/end frame calls
        // integrated with the renderer's UI rendering step.
        m_ui_manager->beginFrame(); // Start ImGui frame
        m_ui_manager->renderMainMenuBar(); // Draw UI elements
        m_ui_manager->renderModuleUI(); // Draw UI for other modules
        // Let the renderer handle drawing the UI (e.g., ImGui draw data)
        m_renderer->renderUI(); // This might trigger a specific UIRenderPass or handle ImGui directly
        m_ui_manager->endFrame(); // End ImGui frame and potentially trigger platform rendering

        // Platform handles swapping buffers
        m_platform->swapBuffers();
    }

    void Application::shutdown() {
        LOG_INFO("Application shutting down systems...");

        // Order can be important: UI might depend on Renderer, Modules might depend on anything.
        // Shut down in reverse order of initialization generally.

        if (m_module_manager) {
            m_module_manager->shutdownModules();
            m_module_manager.reset(); // Release pointer
        }

        if (m_ui_manager) {
            m_ui_manager->shutdown();
            m_ui_manager.reset();
        }

        // Renderer must be shut down before the platform destroys the graphics context/window
        if (m_renderer) {
            m_renderer->shutdown();
            m_renderer.reset();
        }

        if (m_shader_module) {
            m_shader_module->shutdown();
            m_shader_module.reset();
        }

        // Platform cleans up window and graphics API context (like glfwTerminate)
        if (m_platform) {
            m_platform->shutdown();
            m_platform.reset();
        }

        // Clear context resources ONLY if Application owns them directly
        // If they were passed around, owners are responsible.
        // if(m_app_context.scene) m_app_context.scene->clear(); // Example
        // m_app_context.scene.reset();
        // m_app_context.dispatcher.reset();

        LOG_INFO("Application systems shut down complete.");
    }

    // This might not be needed if modules get context during initialize
    ApplicationContext *Application::getContext() {
        return &m_app_context;
    }
}