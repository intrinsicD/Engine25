target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        Engine.cpp
        Module.cpp
        Application.cpp
        Logger.cpp
        ConfigFile.cpp
)

add_subdirectory(Events)
add_subdirectory(Frameworks)
add_subdirectory(Modules)