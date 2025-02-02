//
// Created by alex on 20.01.25.
//

#include "MeshIo.h"
#include "MeshFaceNormals.h"
#include <filesystem>
#include <gtest/gtest.h>

using namespace Bcg;


class MeshIoTest : public ::testing::Test {
protected:
    Mesh mockMesh;
    std::string filename;
    Vertex v0, v1, v2, v3;
    Face f0, f1;

    void SetUp() override {
        // Create a mock mesh representing a simple triangle
        v0 = mockMesh.add_vertex({0.0, 0.0, 0.0});
        v1 = mockMesh.add_vertex({1.0, 0.0, 0.0});
        v2 = mockMesh.add_vertex({0.0, 1.0, 0.0});
        v3 = mockMesh.add_vertex({0.0, 0.0, 1.0});
        f0 = mockMesh.add_face({v0, v1, v2});
        f1 = mockMesh.add_face({v2, v1, v3});
    }

    void TearDown() override {
        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);
        }
    }

    bool WriteMesh(MeshIo &meshIo, MeshIo::WriteFlags flags) {
        bool write_success = meshIo.write(mockMesh, flags);
        if (!write_success) {
            std::cout << "Failed to write mesh to file\n";
        }
        EXPECT_TRUE(write_success);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return write_success;
    }

    bool ReadMesh(MeshIo &meshIo, Mesh &loadedMesh) {
        bool read_success = meshIo.read(loadedMesh);
        if (!read_success) {
            std::cout << "Failed to read mesh from file\n";
        }
        EXPECT_TRUE(read_success);
        return read_success;
    }

    void ValidateMesh(const Mesh &loadedMesh) {
        EXPECT_EQ(mockMesh.n_vertices(), loadedMesh.n_vertices());
        EXPECT_EQ(mockMesh.n_faces(), loadedMesh.n_faces());

        for (const auto &v : loadedMesh.vertices) {
            EXPECT_EQ(mockMesh.positions[v], loadedMesh.positions[v]);
        }
        for (const auto &f : loadedMesh.faces) {
            std::vector<unsigned int> mockFaceVertices;
            std::vector<unsigned int> loadedFaceVertices;
            for (const auto &v : mockMesh.get_vertices(f)) {
                mockFaceVertices.push_back(v.idx());
            }
            for (const auto &v : loadedMesh.get_vertices(f)) {
                loadedFaceVertices.push_back(v.idx());
            }

            Eigen::Map<Vector<unsigned int, 3>> mF(mockFaceVertices.data(), 3);
            Eigen::Map<Vector<unsigned int, 3>> lF(loadedFaceVertices.data(), 3);
            EXPECT_EQ(mF, lF);
        }
    }
};

TEST_F(MeshIoTest, MeshIoOFF_WriteAsciiAndReadFileSuccessfully) {
    filename = "test_triangle.off";
    MeshIoOFF meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = false; // Test ASCII mode

    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST_F(MeshIoTest, MeshIoOFF_WriteBinaryAndReadFileSuccessfully) {
    filename = "test_triangle.off";
    MeshIoOFF meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = true; // Test binary mode

    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST_F(MeshIoTest, MeshIoOBJ_WriteAndReadFileSuccessfully) {
    filename = "test_triangle.obj";
    MeshIoOBJ meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = false; // Test ASCII mode

    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}


TEST_F(MeshIoTest, MeshIoSTL_AsciiWriteAndReadFileSuccessfully) {
    filename = "test_triangle.stl";
    MeshIoSTL meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = false; // Test ASCII mode

    MeshFaceNormals F_normals(mockMesh);

    F_normals.compute();
    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST_F(MeshIoTest, MeshIoSTL_BinaryWriteAndReadFileSuccessfully) {
    filename = "test_triangle.stl";
    MeshIoSTL meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = true; // Test Binary mode

    MeshFaceNormals F_normals(mockMesh);

    F_normals.compute();
    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST_F(MeshIoTest, MeshIoPLY_AsciiWriteAndReadFileSuccessfully) {
    filename = "test_triangle.ply";
    MeshIoPLY meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = false; // Test ASCII mode

    MeshFaceNormals F_normals(mockMesh);

    F_normals.compute();
    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST_F(MeshIoTest, MeshIoPLY_BinaryWriteAndReadFileSuccessfully) {
    filename = "test_triangle.ply";
    MeshIoPLY meshIo(filename);
    MeshIo::WriteFlags flags;
    flags.as_binary = true; // Test binary mode

    MeshFaceNormals F_normals(mockMesh);

    F_normals.compute();
    if (WriteMesh(meshIo, flags)) {
        Mesh loadedMesh;
        if (ReadMesh(meshIo, loadedMesh)) {
            ValidateMesh(loadedMesh);
        }
    }
}

TEST(MeshIoOFF, CanLoadFileWithOffExtension) {
    MeshIoOFF io("test.off");
    EXPECT_TRUE(io.can_load_file());
}

TEST(MeshIoOFF, CannotLoadFileWithNonOffExtension) {
    MeshIoOFF io("test.obj");
    EXPECT_FALSE(io.can_load_file());
}

TEST(MeshIoOBJ, CanLoadFileWithObjExtension) {
    MeshIoOBJ io("test.obj");
    EXPECT_TRUE(io.can_load_file());
}

TEST(MeshIoOBJ, CannotLoadFileWithNonObjExtension) {
    MeshIoOBJ io("test.off");
    EXPECT_FALSE(io.can_load_file());
}

TEST(MeshIoSTL, CanLoadFileWithStlExtension) {
    MeshIoSTL io("test.stl");
    EXPECT_TRUE(io.can_load_file());
}

TEST(MeshIoSTL, CannotLoadFileWithNonStlExtension) {
    MeshIoSTL io("test.off");
    EXPECT_FALSE(io.can_load_file());
}

TEST(MeshIoPLY, CanLoadFileWithPLYExtension) {
    MeshIoPLY io("test.ply");
    EXPECT_TRUE(io.can_load_file());
}

TEST(MeshIoPLY, CannotLoadFileWithNonPLYExtension) {
    MeshIoPLY io("test.off");
    EXPECT_FALSE(io.can_load_file());
}

