//
// Created by alex on 20.01.25.
//

#include "MeshIo.h"
#include <gtest/gtest.h>

using namespace Bcg;

TEST(MeshIoOFF, CanLoadFileWithOffExtension) {
    MeshIoOFF io("test.off");
    EXPECT_TRUE(io.can_load_file());
}

TEST(MeshIoOFF, CannotLoadFileWithNonOffExtension) {
    MeshIoOFF io("test.obj");
    EXPECT_FALSE(io.can_load_file());
}

TEST(MeshIoOFF, WriteAsciiANdReadFileSuccessfully) {
// Create a mock mesh representing a simple triangle
    Mesh mockMesh;

    // Add vertices

    auto v0 = mockMesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mockMesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mockMesh.add_vertex({0.0, 1.0, 0.0});

    // Add a face
    mockMesh.add_face({v0, v1, v2});

    // Set up the MeshIo instance
    MeshIoOFF meshIo("test_triangle.off");
    MeshIo::WriteFlags flags;
    flags.as_binary = false; // Test ASCII mode

    // Write the mesh to an OFF file
    bool write_success = meshIo.write(mockMesh, flags);
    assert(write_success && "Failed to write mesh to OFF file");

    //make sure the file is written
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    // Read the mesh back from the OFF file
    Mesh loadedMesh;
    bool read_success = meshIo.read(loadedMesh);
    assert(read_success && "Failed to read mesh from OFF file");

    // Validate the loaded mesh
    assert(mockMesh.n_vertices() == loadedMesh.n_vertices() && "Vertex count mismatch");
    assert(mockMesh.n_faces() == loadedMesh.n_faces() && "Face count mismatch");

    for (const auto &v : loadedMesh.vertices) {
        assert(mockMesh.positions[v] == loadedMesh.positions[v] && "Vertex position mismatch");
    }

    std::cout << "Test passed: Mesh successfully written and loaded.\n";
}

TEST(MeshIoOFF, FailsToReadFileWithUnsupportedHeader) {
    Mesh mesh;
    MeshIoOFF io("unsupported_header.off");
    EXPECT_FALSE(io.read(mesh));
}

TEST(MeshIoOFF, WritesFileSuccessfully) {
    Mesh mesh;
    MeshIoOFF io("output.off");
    EXPECT_TRUE(io.write(mesh, {}));
}