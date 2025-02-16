target_include_directories(Engine25 PUBLIC .)

if (USE_OPENGL)
    target_include_directories(Engine25 PUBLIC OpenGL)
    target_sources(Engine25 PRIVATE
            OpenGL/OpenGLGraphicsApi.cpp
    )
#[[elseif (USE_VULKAN)
    target_include_directories(Engine25 PUBLIC Vulkan)
    target_sources(Engine25 PRIVATE
            Vulkan/VulkanGraphicsApi.cpp
    )
elseif (USE_DIRECTX)
    target_include_directories(Engine25 PUBLIC DirectX)
    target_sources(Engine25 PRIVATE
            DirectX/DirectXGraphicsApi.cpp
    )]]
endif ()