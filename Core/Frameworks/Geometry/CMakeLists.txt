target_include_directories(Engine25 PUBLIC .)
target_sources(Engine25 PRIVATE
        PointCloud.cpp
        Graph.cpp
        GraphUtils.cpp
        Mesh.cpp
        MeshIo.cpp
        MeshUtils.cpp
        Tree.cpp
        TreeUtils.cpp
        VoxelGrid.cpp
        VoxelGridDownsampling.cpp
)