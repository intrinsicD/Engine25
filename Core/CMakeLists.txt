target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        Application.cpp
        ModuleManager.cpp
        OpenglRenderer.cpp
        UIManager.cpp
        RendererFactory.cpp
        Logger.cpp
        ConfigFile.cpp
        CudaUtils.cpp
        Platform.cpp
        IRenderer.cpp
)

#[[
target_sources(Engine25 PRIVATE
        Engine.cpp
        Module.cpp
        Logger.cpp
        ConfigFile.cpp
)

add_subdirectory(Events)
add_subdirectory(Frameworks)
add_subdirectory(Modules)]]
