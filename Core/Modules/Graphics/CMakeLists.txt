target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        GuiModule.cpp
        GLFWModule.cpp
        RenderingModule.cpp
        SceneModule.cpp
)