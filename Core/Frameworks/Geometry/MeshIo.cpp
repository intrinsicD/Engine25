//
// Created by alex on 20.01.25.
//

#include "MeshIo.h"
#include <filesystem>
#include <iostream>

namespace Bcg {
    bool MeshIoOFF::can_load_file() {
        return std::filesystem::path(m_filename).extension() == ".off";
    }

    bool ReadOFFBinary(std::ifstream &file, Mesh &mesh, VertexProperty<Vector<Real, 3>> normals,
                       VertexProperty<Vector<Real, 3>> colors, VertexProperty<Vector<Real, 2>> texcoords) {
        size_t numVertices, numFaces, numEdges;
        file.read(reinterpret_cast<char *>(&numVertices), sizeof(size_t));
        file.read(reinterpret_cast<char *>(&numFaces), sizeof(size_t));
        file.read(reinterpret_cast<char *>(&numEdges), sizeof(size_t));

        mesh.vertices.resize(numVertices);
        mesh.faces.reserve(numFaces);
        size_t num_edges_upper_bound = std::max(3 * numVertices, numEdges);
        mesh.halfedges.reserve(2 * num_edges_upper_bound);
        mesh.edges.reserve(num_edges_upper_bound);

        // Read vertices
        for (const auto &v: mesh.vertices) {
            file.read(reinterpret_cast<char *>(&mesh.positions[v]), sizeof(Vector<Real, 3>));
            if (normals) {
                file.read(reinterpret_cast<char *>(&normals[v]), sizeof(Vector<Real, 3>));
            }
            if (colors) {
                file.read(reinterpret_cast<char *>(&colors[v]), sizeof(Vector<Real, 3>));
            }
            if (texcoords) {
                file.read(reinterpret_cast<char *>(&texcoords[v]), sizeof(Vector<Real, 2>));
            }
        }

        // Read faces
        for (size_t i = 0; i < numFaces; ++i) {
            size_t faceSize;
            file.read(reinterpret_cast<char *>(&faceSize), sizeof(size_t));

            std::vector<Vertex> indices(faceSize);
            for (size_t j = 0; j < faceSize; ++j) {
                file.read(reinterpret_cast<char *>(&indices[j]), sizeof(size_t));
            }
            mesh.add_face(indices);
        }
        file.close();
        return true;
    }

    bool ReadOFFAscii(std::ifstream &file, Mesh &mesh, VertexProperty<Vector<Real, 3>> normals,
                      VertexProperty<Vector<Real, 3>> colors, VertexProperty<Vector<Real, 2>> texcoords) {
        size_t numVertices, numFaces, numEdges;
        file >> numVertices >> numFaces >> numEdges;

        mesh.vertices.resize(numVertices);
        mesh.faces.reserve(numFaces);
        size_t num_edges_upper_bound = std::max(3 * numVertices, numEdges);
        mesh.halfedges.reserve(2 * num_edges_upper_bound);
        mesh.edges.reserve(num_edges_upper_bound);

        // Read vertices
        for (const auto &v: mesh.vertices) {
            Vector<Real, 3> &pos = mesh.positions[v];
            file >> pos[0] >> pos[1] >> pos[2];

            if (normals) {
                Vector<Real, 3> &normal = normals[v];
                file >> normal[0] >> normal[1] >> normal[2];
            }

            if (colors) {
                Vector<Real, 3> &color = colors[v];
                file >> color[0] >> color[1] >> color[2];
            }

            if (texcoords) {
                Vector<Real, 2> &tex = texcoords[v];
                file >> tex[0] >> tex[1];
            }
        }

        // Read faces
        for (size_t i = 0; i < numFaces; ++i) {
            size_t faceSize;
            file >> faceSize;

            std::vector<Vertex> indices(faceSize);
            for (size_t j = 0; j < faceSize; ++j) {
                size_t index;
                file >> index;
                indices[j] = Vertex(index);
            }
            mesh.add_face(indices);
        }
        file.close();
        return true;
    }

    bool MeshIoOFF::read(Mesh &mesh) {
        std::ifstream file("filename.off", std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: MeshIoOff::read: Could not open file for reading." << std::endl;
            return false;
        }

        std::string line;
        std::getline(file, line);
        std::istringstream header_stream(line);

        bool has_texcoords = false;
        bool has_normals = false;
        bool has_colors = false;
        bool has_hcoords = false;
        bool has_dim = false;
        bool is_binary = false;

        std::string token;
        while (header_stream >> token) {
            if (token == "ST") {
                has_texcoords = true;
            } else if (token == "C") {
                has_colors = true;
            } else if (token == "N") {
                has_normals = true;
            } else if (token == "4") {
                has_hcoords = true;
            } else if (token == "n") {
                has_dim = true;
            } else if (token == "OFF") {
                // Standard OFF header
            } else if (token == "BINARY") {
                is_binary = true;
            } else {
                std::cerr << "Error: Unrecognized token in header: " << token << std::endl;
                return false;
            }
        }

        // Homogeneous coordinates and vertex dimension != 3 are not supported
        if (has_hcoords || has_dim) {
            std::cerr << "Error: Homogeneous coordinates or non-3D vertices are not supported." << std::endl;
            return false;
        }

        VertexProperty<Vector<Real, 3>> normals;
        if (has_normals) {
            normals = mesh.vertex_property<Vector<Real, 3>>("v:normal");
        }

        VertexProperty<Vector<Real, 3>> colors;
        if (has_colors) {
            colors = mesh.vertex_property<Vector<Real, 3>>("v:color");
        }

        VertexProperty<Vector<Real, 2>> texcoords;
        if (has_texcoords) {
            texcoords = mesh.vertex_property<Vector<Real, 2>>("v:tex");
        }

        if (is_binary) {
            return ReadOFFBinary(file, mesh, normals, colors, texcoords);
        } else {
            return ReadOFFAscii(file, mesh, normals, colors, texcoords);
        }
    }

    bool WriteOffBinary(std::ofstream &out, const Mesh &mesh, const MeshIo::WriteFlags &flags){
// Determine the presence of optional attributes
        VertexProperty<Vector<Real, 3>> positions = mesh.get_vertex_property<Vector<Real, 3>>("v:point");
        VertexProperty<Vector<Real, 3>> normals = mesh.get_vertex_property<Vector<Real, 3>>("v:normal");
        VertexProperty<Vector<Real, 3>> colors = mesh.get_vertex_property<Vector<Real, 3>>("v:color");
        VertexProperty<Vector<Real, 2>> texcoords = mesh.get_vertex_property<Vector<Real, 2>>("v:tex");

        // Write header
        size_t numVertices = mesh.vertices.size();
        size_t numFaces = mesh.faces.size();

        out.write(reinterpret_cast<const char *>("OFF"), 3);
        out.write(reinterpret_cast<const char *>(&numVertices), sizeof(size_t));
        out.write(reinterpret_cast<const char *>(&numFaces), sizeof(size_t));

        // Write vertex data
        for (const auto &v: mesh.vertices) {
            const Vector<Real, 3> &pos = positions[v];
            out.write(reinterpret_cast<const char *>(&pos), sizeof(Vector<Real, 3>));

            if (normals) {
                const Vector<Real, 3> &normal = normals[v];
                out.write(reinterpret_cast<const char *>(&normal), sizeof(Vector<Real, 3>));
            }
            if (colors) {
                const Vector<Real, 3> &color = colors[v];
                out.write(reinterpret_cast<const char *>(&color), sizeof(Vector<Real, 3>));
            }
            if (texcoords) {
                const Vector<Real, 2> &texcoord = texcoords[v];
                out.write(reinterpret_cast<const char *>(&texcoord), sizeof(Vector<Real, 2>));
            }
        }

        // Write face data
        FaceProperty<Vector<unsigned int, 3>> tris = mesh.get_face_property<Vector<unsigned int, 3>>("f:triangles");
        if (tris) {
            for (const auto &f: mesh.faces) {
                const Vector<unsigned int, 3> &tri = tris[f];
                out.write(reinterpret_cast<const char *>(&tri), sizeof(Vector<unsigned int, 3>));
            }
        } else {
            for (const auto &f: mesh.faces) {
                auto valence = mesh.get_valence(f);
                out.write(reinterpret_cast<const char *>(&valence), sizeof(size_t));

                for (const auto &v: mesh.get_vertices(f)) {
                    auto idx = v.idx();
                    out.write(reinterpret_cast<const char *>(&idx), sizeof(size_t));
                }
                out << std::endl;
            }
        }

        out.close();
        return true;
    }

    bool WriteOffAscii(std::ofstream &out,const Mesh &mesh, const MeshIo::WriteFlags &flags){
        // Determine the presence of optional attributes
        VertexProperty<Vector<Real, 3>> positions = mesh.get_vertex_property<Vector<Real, 3>>("v:point");
        VertexProperty<Vector<Real, 3>> normals = mesh.get_vertex_property<Vector<Real, 3>>("v:normal");
        VertexProperty<Vector<Real, 3>> colors = mesh.get_vertex_property<Vector<Real, 3>>("v:color");
        VertexProperty<Vector<Real, 2>> texcoords = mesh.get_vertex_property<Vector<Real, 2>>("v:tex");

        // Write header
        out << "OFF" << std::endl;
        out << mesh.vertices.size() << " " << mesh.faces.size() << " 0" << std::endl;

        // Write vertex data
        for (const auto &v: mesh.vertices) {
            const Vector<Real, 3> &pos = positions[v];
            out << std::fixed << std::setprecision(6) << pos.x() << " " << pos.y() << " " << pos.z();

            if (normals) {
                const Vector<Real, 3> &normal = normals[v];
                out << " " << normal.x() << " " << normal.y() << " " << normal.z();
            }
            if (colors) {
                const Vector<Real, 3> &color = colors[v];
                out << " " << color.x() << " " << color.y() << " " << color.z();
            }
            if (texcoords) {
                const Vector<Real, 2> &texcoord = texcoords[v];
                out << " " << texcoord.x() << " " << texcoord.y();
            }
            out << std::endl;
        }

        // Write face data
        FaceProperty<Vector<unsigned int, 3>> tris = mesh.get_face_property<Vector<unsigned int, 3>>("f:triangles");
        if (tris) {
            for (const auto &f: mesh.faces) {
                const Vector<unsigned int, 3> &tri = tris[f];
                out << "3 " << tri.x() << " " << tri.y() << " " << tri.z() << std::endl;
            }
        } else {
            for (const auto &f: mesh.faces) {
                auto valence = mesh.get_valence(f);
                out << valence;

                for (const auto &v: mesh.get_vertices(f)) {
                    out << " " << v.idx();
                }
                out << std::endl;
            }
        }

        out.close();
        return true;
    }

    bool MeshIoOFF::write(const Mesh &mesh, const WriteFlags &flags) {
        if(flags.as_binary){
            std::ofstream out(m_filename, std::ios::out | std::ios::binary);
            if (!out.is_open()) {
                std::cerr << "Error: Could not open binary file " << m_filename << " for writing." << std::endl;
                return false;
            }
            return WriteOffBinary(out, mesh, flags);
        }else{
            std::ofstream out(m_filename, std::ios::out);
            if (!out.is_open()) {
                std::cerr << "Error: Could not open file " << m_filename << " for writing." << std::endl;
                return false;
            }
            return WriteOffAscii(out, mesh, flags);
        }
    }
}