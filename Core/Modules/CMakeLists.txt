target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        Module.cpp
)

add_subdirectory(Graphics)