if(EXISTS "${CMAKE_SOURCE_DIR}/ThirdParty/torch/libtorch")

else()
    message(FATAL_ERROR "Subfolder 'libtorch' does not exist. Download it from https://pytorch.org/get-started/locally/ and extract its libtorch forlder here. Check for the correct cuda version and ABI version!")
endif()


# Point to the TorchConfig.cmake location
set(Torch_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/torch/libtorch/share/cmake/Torch")

find_package(Torch REQUIRED)

target_link_libraries(Engine25 PUBLIC "${TORCH_LIBRARIES}")