target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        InputModule.cpp
)

add_subdirectory(Graphics)