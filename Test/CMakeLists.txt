add_executable(Engine25Tests)

target_sources(Engine25Tests PRIVATE
        TestAABB.cpp
        TestSphere.cpp
        TestPointCloud.cpp
        TestGraph.cpp
        TestMesh.cpp
        TestMeshIo.cpp
        TestTree.cpp
        TestVoxelGrid.cpp
        TestVoxelGridDownsampling.cpp
)

target_link_libraries(Engine25Tests PUBLIC Engine25 gtest_main gtest)