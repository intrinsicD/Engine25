target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        CommandBuffer.cpp
        CommandDoubleBuffer.cpp
        ConfigFile.cpp
        Logger.cpp
        MainLoop.cpp
        Keyboard.cpp
)

add_subdirectory(Graphics)
