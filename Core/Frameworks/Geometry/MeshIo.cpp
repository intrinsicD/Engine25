//
// Created by alex on 20.01.25.
//

#include "MeshIo.h"
#include <regex>
#include <fstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fmt/core.h>

// Define a custom hash function for Vector<Real, 3>
namespace std {
    template<>
    struct hash<Bcg::Vector<Bcg::Real, 3> > {
        size_t operator()(const Bcg::Vector<Bcg::Real, 3> &vec) const {
            size_t h1 = std::hash<Bcg::Real>{}(vec[0]);
            size_t h2 = std::hash<Bcg::Real>{}(vec[1]);
            size_t h3 = std::hash<Bcg::Real>{}(vec[2]);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

namespace Bcg {
    bool is_valid_filename(const std::string &filename) {
        // Allow only alphanumeric characters, underscores, hyphens, and dots in filenames
        static const std::regex valid_filename_regex("^[a-zA-Z0-9._/-]+$");
        return std::regex_match(filename, valid_filename_regex);
    }

    bool MeshIoOFF::can_load_file() {
        return std::filesystem::path(m_filename).extension() == ".off";
    }

    bool ReadOFFBinary(std::ifstream &file, Mesh &mesh, VertexProperty<Vector<Real, 3> > normals,
                       VertexProperty<Vector<Real, 3> > colors, VertexProperty<Vector<Real, 2> > texcoords) {
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

    bool ReadOFFAscii(std::ifstream &file, Mesh &mesh, VertexProperty<Vector<Real, 3> > normals,
                      VertexProperty<Vector<Real, 3> > colors, VertexProperty<Vector<Real, 2> > texcoords) {
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
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoOff::read: Invalid filename." << std::endl;
            return false;
        }

        std::ifstream file(m_filename, std::ios::in | std::ios::binary);
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

        VertexProperty<Vector<Real, 3> > normals;
        if (has_normals) {
            normals = mesh.vertex_property<Vector<Real, 3> >("v:normal");
        }

        VertexProperty<Vector<Real, 3> > colors;
        if (has_colors) {
            colors = mesh.vertex_property<Vector<Real, 3> >("v:color");
        }

        VertexProperty<Vector<Real, 2> > texcoords;
        if (has_texcoords) {
            texcoords = mesh.vertex_property<Vector<Real, 2> >("v:tex");
        }

        mesh.clear();

        if (is_binary) {
            return ReadOFFBinary(file, mesh, normals, colors, texcoords);
        } else {
            return ReadOFFAscii(file, mesh, normals, colors, texcoords);
        }
    }

    bool WriteOffBinary(std::ofstream &out, const Mesh &mesh, const MeshIo::WriteFlags &flags) {
        // Determine the presence of optional attributes
        VertexProperty<Vector<Real, 3> > positions = mesh.get_vertex_property<Vector<Real, 3> >("v:point");
        VertexProperty<Vector<Real, 3> > normals = mesh.get_vertex_property<Vector<Real, 3> >("v:normal");
        VertexProperty<Vector<Real, 3> > colors = mesh.get_vertex_property<Vector<Real, 3> >("v:color");
        VertexProperty<Vector<Real, 2> > texcoords = mesh.get_vertex_property<Vector<Real, 2> >("v:tex");

        // Write header
        size_t numVertices = mesh.vertices.size();
        size_t numFaces = mesh.faces.size();
        size_t numEdges = 0; // OFF format does not require the number of edges

        out.write("OFF BINARY\n", 11);
        out.write(reinterpret_cast<const char *>(&numVertices), sizeof(size_t));
        out.write(reinterpret_cast<const char *>(&numFaces), sizeof(size_t));
        out.write(reinterpret_cast<const char *>(&numEdges), sizeof(size_t));

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
        for (const auto &f: mesh.faces) {
            auto valence = mesh.get_valence(f);
            out.write(reinterpret_cast<const char *>(&valence), sizeof(size_t));

            for (const auto &v: mesh.get_vertices(f)) {
                auto idx = v.idx();
                out.write(reinterpret_cast<const char *>(&idx), sizeof(size_t));
            }
        }

        out.close();
        return true;
    }

    bool WriteOffAscii(std::ofstream &out, const Mesh &mesh, const MeshIo::WriteFlags &flags) {
        // Determine the presence of optional attributes
        VertexProperty<Vector<Real, 3> > positions = mesh.get_vertex_property<Vector<Real, 3> >("v:point");
        VertexProperty<Vector<Real, 3> > normals = mesh.get_vertex_property<Vector<Real, 3> >("v:normal");
        VertexProperty<Vector<Real, 3> > colors = mesh.get_vertex_property<Vector<Real, 3> >("v:color");
        VertexProperty<Vector<Real, 2> > texcoords = mesh.get_vertex_property<Vector<Real, 2> >("v:tex");

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
        FaceProperty<Vector<unsigned int, 3> > tris = mesh.get_face_property<Vector<unsigned int, 3> >("f:triangles");
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
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoOff::write: Invalid filename." << std::endl;
            return false;
        }

        bool ok = false;
        bool file_already_exists = std::filesystem::exists(m_filename);
        auto last_write_time = file_already_exists
                                   ? std::filesystem::last_write_time(m_filename)
                                   : std::filesystem::file_time_type{};

        std::ofstream out(m_filename, flags.as_binary ? (std::ios::out | std::ios::binary) : std::ios::out);
        if (!out.is_open()) {
            std::cerr << "Error: Could not open file " << m_filename << " for writing." << std::endl;
            return false;
        }

        ok = flags.as_binary ? WriteOffBinary(out, mesh, flags) : WriteOffAscii(out, mesh, flags);

        if (ok && file_already_exists) {
            if (std::filesystem::last_write_time(m_filename) == last_write_time) {
                std::cerr << "Error: File " << m_filename << " was not written." << std::endl;
                return false;
            }
        } else {
            return ok && std::filesystem::exists(m_filename);
        }

        return ok;
    }

    bool MeshIoOBJ::read(Mesh &mesh) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoObj::read: Invalid filename." << std::endl;
            return false;
        }

        std::ifstream file(m_filename);
        if (!file.is_open()) {
            std::cerr << "Error: MeshIoObj::read: Could not open file for reading." << std::endl;
            return false;
        }

        std::string line, prefix;
        float x, y, z;
        std::vector<Vertex> vertices;
        std::vector<Vector<Real, 2> > allTexCoords; // Individual texture coordinates
        std::vector<int> halfedgeTexIdx; // Texture coordinates sorted for halfedges
        auto normals = mesh.vertex_property<Vector<Real, 3> >("v:normal");
        auto texCoords = mesh.halfedge_property<Vector<Real, 2> >("h:tex");
        bool has_tex_coords = false;
        bool has_normals = false;

        Vertex v;
        // Parse line by line
        while (std::getline(file, line)) {
            // Trim leading spaces
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));

            if (line.empty() || line[0] == '#') {
                continue; // Skip comments and empty lines
            }

            std::istringstream iss(line);
            iss >> prefix;

            if (prefix == "v" && iss >> x >> y >> z) {
                v = mesh.add_vertex(Vector<Real, 3>(x, y, z));
            } else if (prefix == "vn" && iss >> x >> y >> z) {
                has_normals = true;
                normals[v] = Vector<Real, 3>(x, y, z);
            } else if (prefix == "vt" && iss >> x >> y) {
                allTexCoords.emplace_back(x, y);
            } else if (prefix == "f") {
                vertices.clear();
                halfedgeTexIdx.clear();
                std::string vertexData;

                while (iss >> vertexData) {
                    std::istringstream vertexStream(vertexData);
                    std::string vertexIdx, texIdx;

                    if (std::getline(vertexStream, vertexIdx, '/')) {
                        vertices.emplace_back(std::stoi(vertexIdx) - 1);
                    }
                    if (std::getline(vertexStream, texIdx, '/') && !texIdx.empty()) {
                        halfedgeTexIdx.push_back(std::stoi(texIdx) - 1);
                        has_tex_coords = true;
                    }
                }

                Face face = mesh.add_face(vertices);
                if (has_tex_coords && face.is_valid()) {
                    size_t vIdx = 0;
                    for (const auto &halfedge: mesh.get_halfedges(face)) {
                        if (vIdx < halfedgeTexIdx.size() && halfedgeTexIdx[vIdx] < allTexCoords.size()) {
                            texCoords[halfedge] = allTexCoords[halfedgeTexIdx[vIdx]];
                        }
                        ++vIdx;
                    }
                }
            }
        }

        // Remove texture property if not used
        if (!has_tex_coords) {
            mesh.halfedges.remove(texCoords);
        }
        if (!has_normals) {
            mesh.vertices.remove(normals);
        }

        return true;
    }

    bool MeshIoOBJ::write(const Mesh &mesh, const WriteFlags &flags) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoObj::write: Invalid filename." << std::endl;
            return false;
        }

        std::ofstream out(m_filename);
        if (!out.is_open()) {
            return false;
        }

        out << "# OBJ export from BCG\n";

        auto positions = mesh.positions;
        for (const auto &v: mesh.vertices) {
            out << fmt::format("v {:.10f} {:.10f} {:.10f}\n", positions[v][0], positions[v][1], positions[v][2]);
        }

        auto normals = mesh.get_vertex_property<Vector<Real, 3> >("v:normal");
        if (normals) {
            for (const auto &v: mesh.vertices) {
                out << fmt::format("vn {:.10f} {:.10f} {:.10f}\n", normals[v][0], normals[v][1], normals[v][2]);
            }
        }

        auto tex_coord = mesh.get_halfedge_property<Vector<Real, 2> >("h:tex");
        if (tex_coord) {
            for (const auto &h: mesh.halfedges) {
                out << fmt::format("vt {:.10f} {:.10f}\n", tex_coord[h][0], tex_coord[h][1]);
            }
        }

        for (const auto &f: mesh.faces) {
            out << "f";
            auto fvit = mesh.get_vertices(f);
            auto fhit = mesh.get_halfedges(f);
            do {
                if (tex_coord) {
                    out << fmt::format(" {}/{}/{}", (*fvit).idx() + 1, (*fhit).idx() + 1, (*fvit).idx() + 1);
                    ++fhit;
                } else {
                    out << fmt::format(" {}/{}", (*fvit).idx() + 1, (*fvit).idx() + 1);
                }
            } while (++fvit != mesh.get_vertices(f));
            out << "\n";
        }

        return true;
    }

    bool MeshIoOBJ::can_load_file() {
        return std::filesystem::path(m_filename).extension() == ".obj";
    }


    bool parse_binary_stl(std::ifstream &file, Mesh &mesh) {
        std::array<float, 3> p;
        Vertex v;
        std::vector<Vertex> vertices(3);

        auto cmp = [](const Vector<Real, 3> &a, const Vector<Real, 3> &b) {
            return a.minCoeff() < b.minCoeff();
        };
        std::unordered_map<Vector<Real, 3>, Vertex> vertex_map;

        // Skip the header
        file.seekg(80, std::ios::beg);

        // Read the number of triangles
        uint32_t num_triangles = 0;
        file.read(reinterpret_cast<char *>(&num_triangles), sizeof(num_triangles));

        for (uint32_t t = 0; t < num_triangles; ++t) {
            // Skip triangle normal
            file.seekg(12, std::ios::cur);

            // Process triangle vertices
            for (size_t i = 0; i < 3; ++i) {
                file.read(reinterpret_cast<char *>(p.data()), sizeof(float) * 3);

                Vector<Real, 3> point = {p[0], p[1], p[2]};
                auto [it, inserted] = vertex_map.try_emplace(point, Vertex());
                if (inserted) {
                    v = mesh.add_vertex(point);
                    it->second = v;
                }
                vertices[i] = it->second;
            }

            // Add face if not degenerate
            if ((vertices[0] != vertices[1]) &&
                (vertices[0] != vertices[2]) &&
                (vertices[1] != vertices[2])) {
                mesh.add_face(vertices);
            }

            // Skip attribute byte count
            file.seekg(2, std::ios::cur);
        }

        return true;
    }

    bool parse_ascii_stl(std::ifstream &file, Mesh &mesh) {
        std::array<float, 3> p;
        Vertex v;
        std::vector<Vertex> vertices(3);

        auto cmp = [](const Vector<Real, 3> &a, const Vector<Real, 3> &b) {
            return a.minCoeff() < b.minCoeff();
        };
        std::unordered_map<Vector<Real, 3>, Vertex> vertex_map;

        std::string line;
        while (std::getline(file, line)) {
            // Trim leading whitespace
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));

            if (line.rfind("outer", 0) == 0) {
                // Read three vertices
                for (size_t i = 0; i < 3; ++i) {
                    if (!std::getline(file, line)) {
                        return false;
                    }

                    // Trim leading whitespace
                    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                        return !std::isspace(ch);
                    }));

                    std::istringstream iss(line);
                    std::string dummy;
                    iss >> dummy >> p[0] >> p[1] >> p[2];

                    Vector<Real, 3> point = {p[0], p[1], p[2]};
                    auto [it, inserted] = vertex_map.try_emplace(point, Vertex());
                    if (inserted) {
                        v = mesh.add_vertex(point);
                        it->second = v;
                    }
                    vertices[i] = it->second;
                }

                // Add face if not degenerate
                if ((vertices[0] != vertices[1]) &&
                    (vertices[0] != vertices[2]) &&
                    (vertices[1] != vertices[2])) {
                    mesh.add_face(vertices);
                }
            }
        }

        return true;
    }

    bool MeshIoSTL::read(Mesh &mesh) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoStl::read: Invalid filename." << std::endl;
            return false;
        }

        std::ifstream file(m_filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Determine ASCII or binary STL
        char header[6] = {};
        file.read(header, 5);
        file.seekg(0); // Reset file pointer
        const bool is_binary = (strncmp(header, "solid", 5) != 0);

        if (is_binary) {
            return parse_binary_stl(file, mesh);
        } else {
            file.close(); // Reopen as text
            file.open(m_filename, std::ios::in);
            return parse_ascii_stl(file, mesh);
        }
    }

    bool MeshIoSTL::write(const Mesh &mesh, const WriteFlags &flags) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoStl::write: Invalid filename." << std::endl;
            return false;
        }
        // Check if the mesh is a triangle mesh
        if (!mesh.is_triangle_mesh()) {
            std::cerr << "write_stl: not a triangle mesh!\n";
            return false;
        }

        // Check if face normals are available
        auto fnormals = mesh.get_face_property<Vector<Real, 3> >("f:normal");
        if (!fnormals) {
            std::cerr << "write_stl: no face normals present!\n";
            return false;
        }

        // Open the output file
        std::ofstream ofs(m_filename);
        if (!ofs.is_open()) {
            std::cerr << "write_stl: failed to open file for writing: " << m_filename << "\n";
            return false;
        }

        // Retrieve vertex positions
        auto positions = mesh.positions;
        if (!positions) {
            std::cerr << "write_stl: no vertex positions available!\n";
            return false;
        }

        // Write STL header
        ofs << "solid stl" << std::endl;
        ofs.precision(10);
        ofs.setf(std::ios::fixed);

        // Write facets
        for (const auto &f: mesh.faces) {
            const auto &n = fnormals[f];
            ofs << "  facet normal " << n[0] << " " << n[1] << " " << n[2] << std::endl;
            ofs << "    outer loop" << std::endl;

            for (const auto &v: mesh.get_vertices(f)) {
                const auto &p = positions[v];
                ofs << "      vertex " << p[0] << " " << p[1] << " " << p[2] << std::endl;
            }

            ofs << "    endloop" << std::endl;
            ofs << "  endfacet" << std::endl;
        }

        // Write STL footer
        ofs << "endsolid" << std::endl;

        // Close the file
        ofs.close();
        return true;
    }

    bool MeshIoSTL::can_load_file() {
        return std::filesystem::path(m_filename).extension() == ".stl";
    }


    bool readBinaryPly(std::ifstream &file, Mesh &mesh, size_t vertexCount, size_t faceCount) {
        // Read vertex data
        mesh.vertices.resize(vertexCount);
        for (size_t i = 0; i < vertexCount; ++i) {
            file.read(reinterpret_cast<char *>(&mesh.positions[Vertex(i)]),
                      sizeof(decltype(mesh.positions[Vertex(i)])));
        }

        // Read face data
        for (size_t i = 0; i < faceCount; ++i) {
            unsigned char faceSize;
            file.read(reinterpret_cast<char *>(&faceSize), sizeof(unsigned char));
            std::vector<int> indices(faceSize);
            file.read(reinterpret_cast<char *>(indices.data()), faceSize * sizeof(int));
            std::vector<Vertex> vertex_handles(faceSize);
            std::transform(indices.begin(), indices.end(), vertex_handles.begin(), [](int idx) { return Vertex(idx); });
            mesh.add_face(vertex_handles);
        }

        return true;
    }

    bool readAsciiPly(std::ifstream &file, Mesh &mesh, size_t vertexCount, size_t faceCount) {
        // Read vertex data
        mesh.vertices.reserve(vertexCount);
        for (size_t i = 0; i < vertexCount; ++i) {
            Vector<Real, 3> pos;
            file >> pos[0] >> pos[1] >> pos[2];
            mesh.add_vertex(pos);
        }

        // Read face data
        for (size_t i = 0; i < faceCount; ++i) {
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

        return true;
    }

    bool MeshIoPLY::read(Mesh &mesh) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoPLY::read: Invalid filename." << std::endl;
            return false;
        }

        std::ifstream file(m_filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: MeshIoPLY::read: Could not open file for reading." << std::endl;
            return false;
        }

        std::string line;
        bool isBinary = false;
        std::unordered_map<std::string, size_t> properties;
        size_t vertexCount = 0;
        size_t faceCount = 0;

        // Parse the header
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "ply") {
                continue;
            } else if (token == "format") {
                std::string format;
                iss >> format;
                if (format == "ascii") {
                    isBinary = false;
                } else if (format == "binary_little_endian") {
                    isBinary = true;
                } else {
                    std::cerr << "Error: MeshIoPLY::read: Unsupported PLY format." << std::endl;
                    return false;
                }
            } else if (token == "element") {
                std::string elementType;
                size_t count;
                iss >> elementType >> count;
                if (elementType == "vertex") {
                    vertexCount = count;
                } else if (elementType == "face") {
                    faceCount = count;
                }
            } else if (token == "property") {
                std::string dataType, propertyName;
                iss >> dataType >> propertyName;
                properties[propertyName] = properties.size();
            } else if (token == "end_header") {
                break;
            }
        }

        if (isBinary) {
            return readBinaryPly(file, mesh, vertexCount, faceCount);
        } else {
            return readAsciiPly(file, mesh, vertexCount, faceCount);
        }
    }

    bool MeshIoPLY::write(const Mesh &mesh, const WriteFlags &flags) {
        if (!is_valid_filename(m_filename) || !can_load_file()) {
            std::cerr << "Error: MeshIoPly::write: Invalid filename." << std::endl;
            return false;
        }

        bool isBinary = flags.as_binary;
        std::ofstream file(m_filename, std::ios::out | (isBinary ? std::ios::binary : std::ios::out));
        if (!file.is_open()) {
            std::cerr << "Error: MeshIoPly::write: Could not open file for writing." << std::endl;
            return false;
        }

        // Write header
        file << "ply\n";
        file << (isBinary ? "format binary_little_endian 1.0\n" : "format ascii 1.0\n");
        file << "element vertex " << mesh.vertices.size() << "\n";
        file << "property float x\n";
        file << "property float y\n";
        file << "property float z\n";

        auto normals = mesh.get_vertex_property<Vector<Real, 3> >("v:normal");
        bool with_normals = flags.with_normals && normals;

        auto colors = mesh.get_vertex_property<Vector<Real, 3> >("v:color");
        bool with_colors = flags.with_colors && colors;

        if (with_normals) {
            file << "property float nx\n";
            file << "property float ny\n";
            file << "property float nz\n";
        }

        if (with_colors) {
            file << "property uchar red\n";
            file << "property uchar green\n";
            file << "property uchar blue\n";
            file << "property uchar alpha\n";
        }

        file << "element face " << mesh.faces.size() << "\n";
        file << "property list uchar int vertex_indices\n";
        file << "end_header\n";

        // Write vertex data
        for (size_t i = 0; i < mesh.vertices.size(); ++i) {
            const auto &pos = mesh.positions[Vertex(i)];
            if (isBinary) {
                file.write(reinterpret_cast<const char *>(&pos[0]), sizeof(float));
                file.write(reinterpret_cast<const char *>(&pos[1]), sizeof(float));
                file.write(reinterpret_cast<const char *>(&pos[2]), sizeof(float));
            } else {
                file << pos[0] << " " << pos[1] << " " << pos[2];
            }

            if (with_normals) {
                const auto &normal = normals[Vertex(i)];
                if (isBinary) {
                    file.write(reinterpret_cast<const char *>(&normal[0]), sizeof(float));
                    file.write(reinterpret_cast<const char *>(&normal[1]), sizeof(float));
                    file.write(reinterpret_cast<const char *>(&normal[2]), sizeof(float));
                } else {
                    file << " " << normal[0] << " " << normal[1] << " " << normal[2];
                }
            }

            if (with_colors) {
                const auto &color = colors[Vertex(i)];
                if (isBinary) {
                    unsigned char r = static_cast<unsigned char>(color[0] * 255);
                    unsigned char g = static_cast<unsigned char>(color[1] * 255);
                    unsigned char b = static_cast<unsigned char>(color[2] * 255);
                    unsigned char a = static_cast<unsigned char>(color[3] * 255);
                    file.write(reinterpret_cast<const char *>(&r), sizeof(unsigned char));
                    file.write(reinterpret_cast<const char *>(&g), sizeof(unsigned char));
                    file.write(reinterpret_cast<const char *>(&b), sizeof(unsigned char));
                    file.write(reinterpret_cast<const char *>(&a), sizeof(unsigned char));
                } else {
                    file << " " << static_cast<int>(color[0] * 255) << " "
                            << static_cast<int>(color[1] * 255) << " "
                            << static_cast<int>(color[2] * 255) << " "
                            << static_cast<int>(color[3] * 255);
                }
            }

            if (!isBinary) {
                file << "\n";
            }
        }

        // Write face data
        for (const auto &f: mesh.faces) {
            unsigned char faceSize = static_cast<unsigned char>(mesh.get_valence(f));
            if (isBinary) {
                file.write(reinterpret_cast<const char *>(&faceSize), sizeof(unsigned char));
                for (const auto &v: mesh.get_vertices(f)) {
                    int vertexIndex = v.idx();
                    file.write(reinterpret_cast<const char *>(&vertexIndex), sizeof(int));
                }
            } else {
                file << static_cast<int>(faceSize);
                for (const auto &v: mesh.get_vertices(f)) {
                    file << " " << v.idx();
                }
                file << "\n";
            }
        }

        file.close();
        return true;
    }

    bool MeshIoPLY::can_load_file() {
        return std::filesystem::path(m_filename).extension() == ".ply";
    }


    void MeshIoManager::add_io(std::shared_ptr<MeshIo> io) {
        m_ios.push_back(io);
    }

    std::shared_ptr<MeshIo> MeshIoManager::get_io(const std::string &filename) {
        for (const auto &io : m_ios) {
            if (io->can_load_file()) {
                return io;
            }
        }
        return nullptr;
    }

    bool MeshIoManager::read(Mesh &mesh) {
        for (const auto &io : m_ios) {
            if (io->can_load_file()) {
                if (io->read(mesh)) {
                    return true;
                }
            }
        }
        return false;

    }

    bool MeshIoManager::write(const Mesh &mesh, const MeshIo::WriteFlags &flags) {
        for (const auto &io : m_ios) {
            if (io->can_load_file()) {
                if (io->write(mesh, flags)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool MeshIoManager::can_load_file() {
        for (const auto &io : m_ios) {
            if (io->can_load_file()) {
                return true;
            }
        }
        return false;
    }
}
