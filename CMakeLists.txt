cmake_minimum_required(VERSION 3.30)
project(Engine25)

set(CMAKE_CXX_STANDARD 20)


add_executable(Engine25)
target_compile_definitions(Engine25 PUBLIC DEBUG)
target_sources(Engine25 PRIVATE main.cpp)


add_subdirectory(Core)
add_subdirectory(ThirdParty)
