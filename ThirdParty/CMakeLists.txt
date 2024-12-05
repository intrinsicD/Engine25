add_subdirectory(fmt)
target_link_libraries(Engine25 PUBLIC fmt::fmt)

add_subdirectory(entt)
target_link_libraries(Engine25 PUBLIC EnTT)

set(GLFW_BUILD_WAYLAND OFF)
add_subdirectory(glfw)
target_link_libraries(Engine25 PUBLIC glfw)

target_include_directories(Engine25 SYSTEM PUBLIC glad/include)
target_sources(Engine25 PRIVATE glad/src/gl.c)

add_subdirectory(json)
target_link_libraries(Engine25 PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(Engine25 SYSTEM PUBLIC imgui)
target_sources(Engine25 PRIVATE
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp)