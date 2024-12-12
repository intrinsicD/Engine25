cmake_minimum_required(VERSION 3.30)
project(Engine25)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source directory path as a definition
set(MY_SOURCE_DIR_PATH "${CMAKE_SOURCE_DIR}")
add_compile_definitions(SOURCE_DIR_PATH=\"${MY_SOURCE_DIR_PATH}\")

# Define the Engine25 executable
add_executable(Engine25)

# Define options for backends
option(USE_VULKAN "Enable Vulkan backend" OFF)
option(USE_OPENGL "Enable OpenGL backend" ON)
option(USE_DIRECTX "Enable DirectX backend" OFF)

# Ensure only one backend is selected
if(USE_VULKAN AND (USE_OPENGL OR USE_DIRECTX))
    message(FATAL_ERROR "Only one backend can be enabled at a time.")
endif()
if(USE_OPENGL AND USE_DIRECTX)
    message(FATAL_ERROR "Only one backend can be enabled at a time.")
endif()

# Define backend-specific macros
if(USE_VULKAN)
    target_compile_definitions(Engine25 PUBLIC USE_VULKAN)
    message(STATUS "Using Vulkan backend")
elseif(USE_OPENGL)
    target_compile_definitions(Engine25 PUBLIC USE_OPENGL)
    message(STATUS "Using OpenGL backend")
elseif(USE_DIRECTX)
    target_compile_definitions(Engine25 PUBLIC USE_DIRECTX)
    message(STATUS "Using DirectX backend")
else()
    message(FATAL_ERROR "No backend selected. Please enable one backend.")
endif()

# Add debugging macro
target_compile_definitions(Engine25 PUBLIC DEBUG)

# Add main source file
target_sources(Engine25 PRIVATE main.cpp)

# Add subdirectories for core functionality and third-party dependencies
add_subdirectory(Core)
add_subdirectory(ThirdParty)

# Optional: Specify where to output build files (for organization)
set_target_properties(Engine25 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)