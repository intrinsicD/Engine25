target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        CommandBuffer.cpp
        CommandDoubleBuffer.cpp
        MainLoop.cpp
        Keyboard.cpp
        JobSystem.cpp
        TaskGraph.cpp
)

add_subdirectory(Geometry)
add_subdirectory(Graphics)
