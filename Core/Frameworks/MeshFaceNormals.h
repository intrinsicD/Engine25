//
// Created by alex on 1/22/25.
//

#ifndef MESHFACENORMALS_H
#define MESHFACENORMALS_H

#include "Mesh.h"

namespace Bcg{
    class MeshFaceNormals {
    public:
        explicit MeshFaceNormals(Mesh &mesh) : mesh(mesh),
                                             f_normals(mesh.face_property<Vector<Real, 3>>("f:normal")) {}

        FaceProperty<Vector<Real, 3>> compute() {
            if (!f_normals) {
                return FaceProperty<Vector<Real, 3>>();
            }
            for (const auto &f : mesh.faces) {
                Vector<Real, 3> normal(0.0, 0.0, 0.0);
                auto vertices = mesh.get_vertices(f);
                const auto n = mesh.get_valence(f);
                auto v_current = vertices.begin();
                auto v_next = v_current;
                ++v_next;
                for (size_t i = 0; i < n; ++i) {
                    if (v_next == vertices.end()) v_next = vertices.begin();
                    const auto &p_current = mesh.positions[*v_current];
                    const auto &p_next = mesh.positions[*v_next];
                    normal[0] += (p_current[1] - p_next[1]) * (p_current[2] + p_next[2]);
                    normal[1] += (p_current[2] - p_next[2]) * (p_current[0] + p_next[0]);
                    normal[2] += (p_current[0] - p_next[0]) * (p_current[1] + p_next[1]);
                    ++v_current;
                    ++v_next;
                }
                f_normals[f] = normal.normalized();
            }
            return f_normals;
        }

        private:
          Mesh &mesh;
          FaceProperty<Vector<Real, 3>> f_normals;
    };
}

#endif //MESHFACENORMALS_H
