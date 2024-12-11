cmake_minimum_required(VERSION 3.30)
project(Engine25)

set(CMAKE_CXX_STANDARD 20)

set(MY_SOURCE_DIR_PATH "${CMAKE_SOURCE_DIR}")
add_compile_definitions(SOURCE_DIR_PATH=\"${MY_SOURCE_DIR_PATH}\")

add_executable(Engine25
        Core/Frameworks/ConfigFile.cpp
        Core/Frameworks/Graphics/IDevice.h
        Core/Frameworks/Graphics/ICommandBuffer.h
        Core/Frameworks/Graphics/IShader.h
        Core/Frameworks/Graphics/IPipeline.h
        Core/Frameworks/Graphics/IBuffer.h
        Core/Frameworks/Graphics/ITexture.h
        Core/Frameworks/Graphics/IFramebuffer.h
        Core/Frameworks/Graphics/IDescriptorSet.h
        Core/Frameworks/Graphics/ISynchronization.h
        Core/Frameworks/Graphics/ISwapchain.h
        Core/Frameworks/Graphics/IQueryObject.h
        Core/Frameworks/Graphics/IRenderPass.h
        Core/Frameworks/Graphics/IStateObjects.h
)
target_compile_definitions(Engine25 PUBLIC DEBUG)
target_sources(Engine25 PRIVATE main.cpp)


add_subdirectory(Core)
add_subdirectory(ThirdParty)
