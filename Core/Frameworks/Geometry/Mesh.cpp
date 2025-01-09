//
// Created by alex on 09.01.25.
//

#include "Mesh.h"

namespace Bcg {
    Mesh::Mesh() {

    }

    Mesh &Mesh::operator=(const Mesh &other) {
        if (this != &other) {

        }
        return *this;
    }

    void Mesh::assign(const Mesh &other) {
        if (this != &other) {

        }
    }

    void Mesh::clear() {

    }

    void Mesh::free_memory() {

    }

    void Mesh::reserve(size_t nvertices, size_t nedges, size_t nfaces, size_t ntets) {

    }

    void Mesh::garbage_collection() {

    }

    // Vertex Methods

    void Mesh::mark_deleted(const Vertex &v) {

    }

    void Mesh::delete_vertex(const Vertex &v) {

    }

    // Halfedge Methods

    void Mesh::mark_deleted(const Halfedge &h) {

    }

    // Edge Methods

    Halfedge Mesh::new_edge(const Vertex &v0, const Vertex &v1) {
        assert(v0 != v1);
        edges.push_back();
        halfedges.push_back();
        halfedges.push_back();

        Halfedge h(halfedges.size() - 2);
        Halfedge o(halfedges.size() - 1);

        set_vertex(h, v1);
        set_vertex(o, v0);
        return h;
    }

    void Mesh::mark_deleted(const Edge &e) {

    }

    void Mesh::delete_edge(const Edge &e) {

    }

    // Face Methods

    Face Mesh::add_face(const Vertex &v0, const Vertex &v1, const Vertex &v2){

    }

    Face Mesh::add_face(const std::vector<Vertex> &f_vertices){

    }

    void Mesh::mark_deleted(const Face &f) {

    }

    void Mesh::delete_face(const Face &f) {

    }

    // Tet Methods

    Tet Mesh::add_tet(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3){

    }

    void Mesh::mark_deleted(const Tet &t){

    }

    void Mesh::delete_tet(const Tet &t){

    }
}