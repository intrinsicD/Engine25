//
// Created by alex on 09.01.25.
//

#include "Mesh.h"
#include "Exceptions.h"
#include "Logger.h"

namespace Bcg {
    Mesh::Mesh() : Graph() {
        positions = vertex_property<Vector<Real, 3> >("v:point");
        v_connectivity = vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedge_property<HalfedgeConnectivity>("h:connectivity");
        f_connectivity = face_property<FaceConnectivity>("f:connectivity");

        v_deleted = vertex_property<bool>("v:deleted", false);
        h_deleted = halfedge_property<bool>("h:deleted", false);
        e_deleted = edge_property<bool>("e:deleted", false);
        f_deleted = face_property<bool>("f:deleted", false);
    }

    Mesh &Mesh::operator=(const Mesh &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;
            halfedges = rhs.halfedges;
            edges = rhs.edges;
            faces = rhs.faces;

            // property handles contain pointers, have to be reassigned
            positions = vertex_property<Vector<Real, 3> >("v:point");
            v_connectivity = vertex_property<VertexConnectivity>("v:connectivity");
            h_connectivity = halfedge_property<HalfedgeConnectivity>("h:connectivity");
            f_connectivity = face_property<FaceConnectivity>("f:connectivity");

            v_deleted = vertex_property<bool>("v:deleted");
            h_deleted = halfedge_property<bool>("h:deleted");
            e_deleted = edge_property<bool>("e:deleted");
            f_deleted = face_property<bool>("f:deleted");

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
            edges.num_deleted = rhs.edges.num_deleted;
            faces.num_deleted = rhs.faces.num_deleted;
        }
        return *this;
    }

    Mesh &Mesh::assign(const Mesh &rhs) {
        if (this != &rhs) {
            // clear properties
            vertices.clear();
            halfedges.clear();
            edges.clear();
            faces.clear();

            // allocate standard properties
            positions = vertex_property<Vector<Real, 3> >("v:point");
            v_connectivity = vertex_property<VertexConnectivity>("v:connectivity");
            h_connectivity = halfedge_property<HalfedgeConnectivity>("h:connectivity");
            f_connectivity = face_property<FaceConnectivity>("f:connectivity");

            v_deleted = vertex_property<bool>("v:deleted", false);
            e_deleted = edge_property<bool>("e:deleted", false);
            f_deleted = face_property<bool>("f:deleted", false);

            // copy properties from other mesh
            positions.vector() = rhs.positions.vector();
            v_connectivity.vector() = rhs.v_connectivity.vector();
            h_connectivity.vector() = rhs.h_connectivity.vector();
            f_connectivity.vector() = rhs.f_connectivity.vector();

            v_deleted.vector() = rhs.v_deleted.vector();
            e_deleted.vector() = rhs.e_deleted.vector();
            f_deleted.vector() = rhs.f_deleted.vector();

            // resize (needed by property containers)
            vertices.resize(rhs.vertices.size());
            halfedges.resize(rhs.halfedges.size());
            edges.resize(rhs.edges.size());
            faces.resize(rhs.faces.size());

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
            edges.num_deleted = rhs.edges.num_deleted;
            faces.num_deleted = rhs.faces.num_deleted;
        }
        return *this;
    }

    void Mesh::clear() {
        // remove all properties
        vertices.clear();
        halfedges.clear();
        edges.clear();
        faces.clear();

        // really free their memory
        free_memory();

        // add the standard properties back
        positions = vertex_property<Vector<Real, 3> >("v:point");
        v_connectivity = vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedge_property<HalfedgeConnectivity>("h:connectivity");
        f_connectivity = face_property<FaceConnectivity>("f:connectivity");
        v_deleted = vertex_property<bool>("v:deleted", false);
        e_deleted = edge_property<bool>("e:deleted", false);
        h_deleted = halfedge_property<bool>("h:deleted", false);
        f_deleted = face_property<bool>("f:deleted", false);

        // set initial status (as in constructor)
        vertices.num_deleted = 0;
        edges.num_deleted = 0;
        halfedges.num_deleted = 0;
        faces.num_deleted = 0;
    }

    void Mesh::free_memory() {
        vertices.free_memory();
        halfedges.free_memory();
        edges.free_memory();
        faces.free_memory();
    }

    void Mesh::reserve(size_t nvertices, size_t nedges, size_t nfaces) {
        Graph::reserve(nvertices, nedges);
        faces.reserve(nfaces);
    }

    void Mesh::garbage_collection() {
        if (!has_garbage()) {
            return;
        }

        auto nV = vertices.size();
        auto nE = edges.size();
        auto nH = halfedges.size();
        auto nF = faces.size();

        // setup handle mapping
        VertexProperty<Vertex> vmap = vertex_property<Vertex>("v:garbage-collection");
        HalfedgeProperty<Halfedge> hmap = halfedge_property<Halfedge>("h:garbage-collection");
        FaceProperty<Face> fmap = face_property<Face>("f:garbage-collection");
        for (size_t i = 0; i < nV; ++i) {
            vmap[Vertex(i)] = Vertex(i);
        }
        for (size_t i = 0; i < nH; ++i) {
            hmap[Halfedge(i)] = Halfedge(i);
        }
        for (size_t i = 0; i < nF; ++i) {
            fmap[Face(i)] = Face(i);
        }

        // remove deleted vertices
        if (nV > 0) {
            size_t i0 = 0;
            size_t i1 = nV - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!v_deleted[Vertex(i0)] && i0 < i1) {
                    ++i0;
                }
                while (v_deleted[Vertex(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) {
                    break;
                }

                // swap
                vertices.swap(i0, i1);
            }

            // remember new size
            nV = v_deleted[Vertex(i0)] ? i0 : i0 + 1;
        }

        // remove deleted edges
        if (nE > 0) {
            size_t i0 = 0;
            size_t i1 = nE - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!e_deleted[Edge(i0)] && i0 < i1) {
                    ++i0;
                }
                while (e_deleted[Edge(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) {
                    break;
                }

                // swap
                edges.swap(i0, i1);
                halfedges.swap(2 * i0, 2 * i1);
                halfedges.swap(2 * i0 + 1, 2 * i1 + 1);
            }

            // remember new size
            nE = e_deleted[Edge(i0)] ? i0 : i0 + 1;
            nH = 2 * nE;
        }

        // remove deleted faces
        if (nF > 0) {
            size_t i0 = 0;
            size_t i1 = nF - 1;

            while (true) {
                // find 1st deleted and last un-deleted
                while (!f_deleted[Face(i0)] && i0 < i1) {
                    ++i0;
                }
                while (f_deleted[Face(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) {
                    break;
                }
                // swap
                faces.swap(i0, i1);
            }

            // remember new size
            nF = f_deleted[Face(i0)] ? i0 : i0 + 1;
        }

        // update vertex connectivity
        for (size_t i = 0; i < nV; ++i) {
            auto v = Vertex(i);
            if (!is_isolated(v))
                Graph::set_halfedge(v, hmap[Graph::get_halfedge(v)]);
        }

        // update halfedge connectivity
        for (size_t i = 0; i < nH; ++i) {
            auto h = Halfedge(i);
            set_vertex(h, vmap[get_vertex(h)]);
            set_next(h, hmap[get_next(h)]);
            if (!is_boundary(h)) {
                set_face(h, fmap[get_face(h)]);
            }
        }

        // update handles of faces
        for (size_t i = 0; i < nF; ++i) {
            auto f = Face(i);
            set_halfedge(f, hmap[get_halfedge(f)]);
        }

        // remove handle maps
        remove_vertex_property(vmap);
        remove_halfedge_property(hmap);
        remove_face_property(fmap);

        // finally resize arrays
        vertices.resize(nV);
        halfedges.resize(nH);
        edges.resize(nE);
        faces.resize(nF);

        free_memory();

        vertices.num_deleted = edges.num_deleted = faces.num_deleted = 0;
    }

    bool Mesh::is_triangle_mesh() const {
        /*
                for (auto f: faces) {
                    if (get_valence(f) != 3) return false;
                }
        */

        return std::all_of(faces.begin(), faces.end(), [this](const Face &f) {
            return get_valence(f) == 3;
        });
    }

    bool Mesh::is_quad_mesh() const {
        for (auto f: faces) {
            if (get_valence(f) != 4) return false;
        }

        return true;
    }

    // Vertex Methods

    bool Mesh::is_manifold(const Vertex &v) const {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        auto hit = get_halfedges(v);
        auto hend = hit;
        if (hit)
            do {
                if (is_boundary(*hit))
                    ++n;
            } while (++hit != hend);
        return n < 2;
    }


    void Mesh::delete_vertex(const Vertex &v) {
        if (PointCloud::is_deleted(v)) {
            return;
        }

        // collect incident faces
        std::vector<Face> incident_faces;
        incident_faces.reserve(6);

        for (auto f: get_faces(v))
            incident_faces.push_back(f);

        // delete incident faces
        for (auto f: incident_faces)
            delete_face(f);

        // mark v as deleted if not yet done by delete_face()
        if (!v_deleted[v]) {
            v_deleted[v] = true;
            ++vertices.num_deleted;
        }
    }

    void Mesh::adjust_outgoing_halfedge(const Vertex &v) {
        Halfedge h = Graph::get_halfedge(v);
        const Halfedge hh = h;

        if (h.is_valid()) {
            do {
                if (is_boundary(h)) {
                    Graph::set_halfedge(v, h);
                    return;
                }
                h = rotate_cw(h);
            } while (h != hh);
        }
    }

    // Halfedge Methods

    Halfedge Mesh::insert_vertex(const Halfedge &h0, const Vertex &v) {
        // before:
        //
        // v0      h0       v2
        //  o--------------->o
        //   <---------------
        //         o0
        //
        // after:
        //
        // v0  h0   v   h1   v2
        //  o------>o------->o
        //   <------ <-------
        //     o0       o1

        Halfedge h2 = get_next(h0);
        Halfedge o0 = get_opposite(h0);
        Halfedge o2 = get_prev(o0);
        Vertex v2 = get_vertex(h0);
        Face fh = get_face(h0);
        Face fo = get_face(o0);

        Halfedge h1 = new_edge(v, v2);
        Halfedge o1 = get_opposite(h1);

        // adjust halfedge connectivity
        set_next(h1, h2);
        set_next(h0, h1);
        set_vertex(h0, v);
        set_vertex(h1, v2);
        set_face(h1, fh);

        set_next(o1, o0);
        set_next(o2, o1);
        set_vertex(o1, v);
        set_face(o1, fo);

        // adjust vertex connectivity
        Graph::set_halfedge(v2, o1);
        adjust_outgoing_halfedge(v2);
        Graph::set_halfedge(v, h1);
        adjust_outgoing_halfedge(v);

        // adjust face connectivity
        if (fh.is_valid()) {
            set_halfedge(fh, h0);
        }
        if (fo.is_valid()) {
            set_halfedge(fo, o1);
        }

        return o1;
    }

    bool Mesh::is_collapse_ok(const Halfedge &h) const {
        Halfedge o = get_opposite(h);
        Vertex v0 = get_vertex(o);
        Vertex v1 = get_vertex(h);
        Vertex vl, vr;
        Halfedge h1, h2;

        // the edges v1-vl and vl-v0 must not be both boundary edges
        if (!is_boundary(h)) {
            vl = get_vertex(get_next(h));
            h1 = get_next(h);
            h2 = get_next(h1);
            if (is_boundary(get_opposite(h1)) &&
                is_boundary(get_opposite(h2)))
                return false;
        }

        // the edges v0-vr and vr-v1 must not be both boundary edges
        if (!is_boundary(o)) {
            vr = get_vertex(get_next(o));
            h1 = get_next(o);
            h2 = get_next(h1);
            if (is_boundary(get_opposite(h1)) &&
                is_boundary(get_opposite(h2)))
                return false;
        }

        // if vl and vr are equal or both invalid -> fail
        if (vl == vr)
            return false;

        // edge between two boundary vertices should be a boundary edge
        if (is_boundary(v0) && is_boundary(v1) && !is_boundary(h) && !is_boundary(o))
            return false;

        // test intersection of the one-rings of v0 and v1
        for (auto vv: get_vertices(v0)) {
            if (vv != v1 && vv != vl && vv != vr)
                if (find_halfedge(vv, v1).is_valid())
                    return false;
        }

        // passed all tests
        return true;
    }

    void Mesh::collapse(const Halfedge &h0) {
        //move v0 to v1
        Halfedge h1 = get_prev(h0);
        Halfedge o0 = get_opposite(h0);
        Halfedge o1 = get_next(o0);

        // remove edge
        remove_edge_helper(h0);

        // remove loops
        if (get_next(get_next(h1)) == h1) {
            remove_loop_helper(h1);
        }
        if (get_next(get_next(o1)) == o1) {
            remove_loop_helper(o1);
        }

        assert(has_garbage());
    }

    void Mesh::remove_edge_helper(const Halfedge &h) {
        Halfedge hn = get_next(h);
        Halfedge hp = get_prev(h);

        Halfedge o = get_opposite(h);
        Halfedge on = get_next(o);
        Halfedge op = get_prev(o);

        Face fh = get_face(h);
        Face fo = get_face(o);

        Vertex vh = get_vertex(h);
        Vertex vo = get_vertex(o);

        // halfedge -> vertex
        HalfedgeAroundVertexCirculator vhit, vhend;
        vhit = vhend = get_halfedges(vo);
        do {
            set_vertex(get_opposite(*vhit), vh);
        } while (++vhit != vhend);

        // halfedge -> halfedge
        set_next(hp, hn);
        set_next(op, on);

        // face -> halfedge
        if (fh.is_valid()) {
            set_halfedge(fh, hn);
        }
        if (fo.is_valid()) {
            set_halfedge(fo, on);
        }

        // vertex -> halfedge
        if (Graph::get_halfedge(vh) == o) {
            Graph::set_halfedge(vh, hn);
        }
        adjust_outgoing_halfedge(vh);
        Graph::set_halfedge(vo, Halfedge());

        PointCloud::mark_deleted(vo);
        Graph::mark_deleted(get_edge(h));

        assert(has_garbage());
    }

    void Mesh::remove_loop_helper(const Halfedge &h0) {
        Halfedge h1 = get_next(h0);

        Halfedge o0 = get_opposite(h0);
        Halfedge o1 = get_opposite(h1);

        Vertex v0 = get_vertex(h0);
        Vertex v1 = get_vertex(h1);

        Face fh = get_face(h0);
        Face fo = get_face(o0);

        // is it a loop ?
        assert((get_next(h1) == h0) && (h1 != o0));

        // halfedge -> halfedge
        set_next(h1, get_next(o0));
        set_next(get_prev(o0), h1);

        // halfedge -> face
        set_face(h1, fo);

        // vertex -> halfedge
        Graph::set_halfedge(v0, h1);
        adjust_outgoing_halfedge(v0);
        Graph::set_halfedge(v1, o1);
        adjust_outgoing_halfedge(v1);

        // face -> halfedge
        if (fo.is_valid() && get_halfedge(fo) == o0) {
            set_halfedge(fo, h1);
        }

        // delete stuff
        if (fh.is_valid()) {
            mark_deleted(fh);
        }

        Graph::mark_deleted(get_edge(h0));

        assert(has_garbage());
    }

    // Edge Methods

    bool Mesh::is_removal_ok(Edge e) const {
        Halfedge h0 = get_halfedge(e, 0);
        Halfedge h1 = get_halfedge(e, 1);
        Vertex v0 = get_vertex(h0);
        Vertex v1 = get_vertex(h1);
        Face f0 = get_face(h0);
        Face f1 = get_face(h1);

        // boundary?
        if (!f0.is_valid() || !f1.is_valid()) {
            return false;
        }

        // same face?
        if (f0 == f1) {
            return false;
        }

        // are the two faces connect through another vertex?
        for (auto v: get_vertices(f0)) {
            if (v != v0 && v != v1) {
                for (auto f: get_faces(v)) {
                    if (f == f1) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void Mesh::delete_edge(const Edge &e) {
        if (Graph::is_deleted(e)) {
            return;
        }

        Face f0 = get_face(get_halfedge(e, 0));
        Face f1 = get_face(get_halfedge(e, 1));

        if (f0.is_valid()) {
            delete_face(f0);
        }
        if (f1.is_valid()) {
            delete_face(f1);
        }
    }


    bool Mesh::remove_edge(Edge e) {
        if (!is_removal_ok(e))
            return false;

        Halfedge h0 = get_halfedge(e, 0);
        Halfedge h1 = get_halfedge(e, 1);

        Vertex v0 = get_vertex(h0);
        Vertex v1 = get_vertex(h1);

        Face f0 = get_face(h0);
        Face f1 = get_face(h1);

        Halfedge h0_prev = get_prev(h0);
        Halfedge h0_next = get_next(h0);
        Halfedge h1_prev = get_prev(h1);
        Halfedge h1_next = get_next(h1);

        // adjust vertex->halfedge
        if (get_halfedge(v0) == h1)
            Graph::set_halfedge(v0, h0_next);
        if (get_halfedge(v1) == h0)
            Graph::set_halfedge(v1, h1_next);

        // adjust halfedge->face
        for (auto h: get_halfedges(f0)) {
            set_face(h, f1);
        }

        // adjust halfedge->halfedge
        set_next(h1_prev, h0_next);
        set_next(h0_prev, h1_next);

        // adjust face->halfedge
        if (get_halfedge(f1) == h1) {
            set_halfedge(f1, h1_next);
        }

        // delete face f0 and edge e
        mark_deleted(f0);
        Graph::mark_deleted(e);

        return true;
    }

    Edge Mesh::find_edge(const Vertex &start, const Vertex &end) const {
        Halfedge h = find_halfedge(start, end);
        return h.is_valid() ? get_edge(h) : Edge();
    }

    bool Mesh::is_flip_ok(const Edge &e) const {
        // boundary edges cannot be flipped
        if (Graph::is_boundary(e))
            return false;

        // check if the flipped edge is already present in the mesh
        Halfedge h0 = get_halfedge(e, 0);
        Halfedge h1 = get_halfedge(e, 1);

        Vertex v0 = get_vertex(get_next(h0));
        Vertex v1 = get_vertex(get_next(h1));

        if (v0 == v1) // this is generally a bad sign !!!
            return false;

        if (find_halfedge(v0, v1).is_valid())
            return false;

        return true;
    }

    void Mesh::flip(const Edge &e) {
        //let's make it sure it is actually checked
        assert(is_flip_ok(e));

        Halfedge a0 = get_halfedge(e, 0);
        Halfedge b0 = get_halfedge(e, 1);

        Halfedge a1 = get_next(a0);
        Halfedge a2 = get_next(a1);

        Halfedge b1 = get_next(b0);
        Halfedge b2 = get_next(b1);

        Vertex va0 = get_vertex(a0);
        Vertex va1 = get_vertex(a1);

        Vertex vb0 = get_vertex(b0);
        Vertex vb1 = get_vertex(b1);

        Face fa = get_face(a0);
        Face fb = get_face(b0);

        set_vertex(a0, va1);
        set_vertex(b0, vb1);

        set_next(a0, a2);
        set_next(a2, b1);
        set_next(b1, a0);

        set_next(b0, b2);
        set_next(b2, a1);
        set_next(a1, b0);

        set_face(a1, fb);
        set_face(b1, fa);

        set_halfedge(fa, a0);
        set_halfedge(fb, b0);

        if (Graph::get_halfedge(va0) == b0)
            Graph::set_halfedge(va0, a1);
        if (Graph::get_halfedge(vb0) == a0)
            Graph::set_halfedge(vb0, b1);
    }


    Halfedge Mesh::split(Edge e, Vertex v) {
        Halfedge h0 = get_halfedge(e, 0);
        Halfedge o0 = get_halfedge(e, 1);

        Vertex v2 = get_vertex(o0);

        Halfedge e1 = new_edge(v, v2);
        Halfedge t1 = get_opposite(e1);

        Face f0 = get_face(h0);
        Face f3 = get_face(o0);

        Graph::set_halfedge(v, h0);
        set_vertex(o0, v);

        if (!is_boundary(h0)) {
            Halfedge h1 = get_next(h0);
            Halfedge h2 = get_next(h1);

            Vertex v1 = get_vertex(h1);

            Halfedge e0 = new_edge(v, v1);
            Halfedge t0 = get_opposite(e0);

            Face f1 = new_face();
            set_halfedge(f0, h0);
            set_halfedge(f1, h2);

            set_face(h1, f0);
            set_face(t0, f0);
            set_face(h0, f0);

            set_face(h2, f1);
            set_face(t1, f1);
            set_face(e0, f1);

            set_next(h0, h1);
            set_next(h1, t0);
            set_next(t0, h0);

            set_next(e0, h2);
            set_next(h2, t1);
            set_next(t1, e0);
        } else {
            set_next(get_prev(h0), t1);
            set_next(t1, h0);
            // halfedge handle of vh already is h0
        }

        if (!is_boundary(o0)) {
            Halfedge o1 = get_next(o0);
            Halfedge o2 = get_next(o1);

            Vertex v3 = get_vertex(o1);

            Halfedge e2 = new_edge(v, v3);
            Halfedge t2 = get_opposite(e2);

            Face f2 = new_face();
            set_halfedge(f2, o1);
            set_halfedge(f3, o0);

            set_face(o1, f2);
            set_face(t2, f2);
            set_face(e1, f2);

            set_face(o2, f3);
            set_face(o0, f3);
            set_face(e2, f3);

            set_next(e1, o1);
            set_next(o1, t2);
            set_next(t2, e1);

            set_next(o0, e2);
            set_next(e2, o2);
            set_next(o2, o0);
        } else {
            set_next(e1, get_next(o0));
            set_next(o0, e1);
            Graph::set_halfedge(v, e1);
        }

        if (get_halfedge(v2) == h0)
            Graph::set_halfedge(v2, t1);

        return t1;
    }

    // Face Methods

    Face Mesh::add_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
        m_add_face_vertices.resize(3);
        m_add_face_vertices[0] = v0;
        m_add_face_vertices[1] = v1;
        m_add_face_vertices[2] = v2;
        return add_face(m_add_face_vertices);
    }

    Face Mesh::add_quad(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3) {
        m_add_face_vertices.resize(4);
        m_add_face_vertices[0] = v0;
        m_add_face_vertices[1] = v1;
        m_add_face_vertices[2] = v2;
        m_add_face_vertices[3] = v3;
        return add_face(m_add_face_vertices);
    }

    Face Mesh::add_face(const std::vector<Vertex> &f_vertices) {
        const size_t n(f_vertices.size());
        assert(n > 2);

        Vertex v;
        size_t i, ii, id;
        Halfedge inner_next, inner_prev, outer_next, outer_prev, boundary_next,
                boundary_prev, patch_start, patch_end;

        // use global arrays to avoid new/delete of local arrays!!!
        std::vector<Halfedge> &halfedges = m_add_face_halfedges;
        std::vector<bool> &is_new = m_add_face_is_new;
        std::vector<bool> &needs_adjust = m_add_face_needs_adjust;
        NextCache &next_cache = m_add_face_next_cache;
        halfedges.clear();
        halfedges.resize(n);
        is_new.clear();
        is_new.resize(n);
        needs_adjust.clear();
        needs_adjust.resize(n, false);
        next_cache.clear();
        next_cache.reserve(3 * n);

        // test for topological errors
        for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (!is_boundary(f_vertices[i])) {
                auto what = "Mesh::add_face: Complex vertex.";
                LOG_ERROR(what);
            }

            halfedges[i] = find_halfedge(f_vertices[i], f_vertices[ii]);
            is_new[i] = !halfedges[i].is_valid();

            if (!is_new[i] && !is_boundary(halfedges[i])) {
                auto what = "Mesh::add_face: Complex edge.";
                LOG_ERROR(what);
            }
        }

        // re-link patches if necessary
        for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (!is_new[i] && !is_new[ii]) {
                inner_prev = halfedges[i];
                inner_next = halfedges[ii];

                if (get_next(inner_prev) != inner_next) {
                    // here comes the ugly part... we have to relink a whole patch

                    // search a free gap
                    // free gap will be between boundaryPrev and boundaryNext
                    outer_prev = get_opposite(inner_next);
                    outer_next = get_opposite(inner_prev);
                    boundary_prev = outer_prev;
                    do {
                        boundary_prev = get_opposite(get_next(boundary_prev));
                    } while (!is_boundary(boundary_prev) || boundary_prev == inner_prev);
                    boundary_next = get_next(boundary_prev);
                    assert(is_boundary(boundary_prev));
                    assert(is_boundary(boundary_next));

                    // ok ?
                    if (boundary_next == inner_next) {
                        auto what = "Mesh::add_face: Patch re-linking failed.";
                        LOG_ERROR(what);
                    }

                    // other halfedges' handles
                    patch_start = get_next(inner_prev);
                    patch_end = get_prev(inner_next);

                    // relink
                    next_cache.emplace_back(boundary_prev, patch_start);
                    next_cache.emplace_back(patch_end, boundary_next);
                    next_cache.emplace_back(inner_prev, inner_next);
                }
            }
        }

        // create missing edges
        for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (is_new[i]) {
                halfedges[i] = new_edge(f_vertices[i], f_vertices[ii]);
            }
        }

        // create the face
        Face f(new_face());
        set_halfedge(f, halfedges[n - 1]);

        // setup halfedges
        for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            v = f_vertices[ii];
            inner_prev = halfedges[i];
            inner_next = halfedges[ii];

            id = 0;
            if (is_new[i])
                id |= 1;
            if (is_new[ii])
                id |= 2;

            if (id) {
                outer_prev = get_opposite(inner_next);
                outer_next = get_opposite(inner_prev);

                // set outer links
                switch (id) {
                    case 1: // prev is new, next is old
                        boundary_prev = get_prev(inner_next);
                        next_cache.emplace_back(boundary_prev, outer_next);
                        Graph::set_halfedge(v, outer_next);
                        break;

                    case 2: // next is new, prev is old
                        boundary_next = get_next(inner_prev);
                        next_cache.emplace_back(outer_prev, boundary_next);
                        Graph::set_halfedge(v, boundary_next);
                        break;

                    case 3: // both are new
                        if (!Graph::get_halfedge(v).is_valid()) {
                            Graph::set_halfedge(v, outer_next);
                            next_cache.emplace_back(outer_prev, outer_next);
                        } else {
                            boundary_next = Graph::get_halfedge(v);
                            boundary_prev = get_prev(boundary_next);
                            next_cache.emplace_back(boundary_prev, outer_next);
                            next_cache.emplace_back(outer_prev, boundary_next);
                        }
                        break;
                }

                // set inner link
                next_cache.emplace_back(inner_prev, inner_next);
            } else
                needs_adjust[ii] = (Graph::get_halfedge(v) == inner_next);

            // set face handle
            set_face(halfedges[i], f);
        }

        // process next halfedge cache
        NextCache::const_iterator ncIt(next_cache.begin()), ncEnd(next_cache.end());
        for (; ncIt != ncEnd; ++ncIt) {
            set_next(ncIt->first, ncIt->second);
        }

        // adjust vertices' halfedge handle
        for (i = 0; i < n; ++i) {
            if (needs_adjust[i]) {
                adjust_outgoing_halfedge(f_vertices[i]);
            }
        }

        return f;
    }

    void Mesh::mark_deleted(const Face &f) {
        if (f_deleted[f]) {
            return;
        }

        f_deleted[f] = true;
        ++faces.num_deleted;
    }

    void Mesh::delete_face(const Face &f) {
        if (f_deleted[f])
            return;

        // mark face deleted
        mark_deleted(f);

        // boundary edges of face f to be deleted
        std::vector<Edge> deletedEdges;
        deletedEdges.reserve(3);

        // vertices of face f for updating their outgoing halfedge
        std::vector<Vertex> vertices_;
        vertices_.reserve(3);

        // for all halfedges of face f do:
        //   1) invalidate face handle.
        //   2) collect all boundary halfedges, set them deleted
        //   3) store vertex handles
        for (auto hc: get_halfedges(f)) {
            set_face(hc, Face());

            if (is_boundary(get_opposite(hc)))
                deletedEdges.push_back(get_edge(hc));

            vertices_.push_back(get_vertex(hc));
        }

        // delete all collected (half)edges
        // delete isolated vertices
        if (!deletedEdges.empty()) {
            auto delit(deletedEdges.begin()), delend(deletedEdges.end());

            Halfedge h0, h1, next0, next1, prev0, prev1;
            Vertex v0, v1;

            for (; delit != delend; ++delit) {
                h0 = get_halfedge(*delit, 0);
                v0 = get_vertex(h0);
                next0 = get_next(h0);
                prev0 = get_prev(h0);

                h1 = get_halfedge(*delit, 1);
                v1 = get_vertex(h1);
                next1 = get_next(h1);
                prev1 = get_prev(h1);

                // adjust next and prev handles
                set_next(prev0, next1);
                set_next(prev1, next0);

                // mark edge deleted
                Graph::mark_deleted(*delit);

                // update v0
                if (Graph::get_halfedge(v0) == h1) {
                    if (next0 == h1) {
                        PointCloud::mark_deleted(v0);
                    } else
                        Graph::set_halfedge(v0, next0);
                }

                // update v1
                if (Graph::get_halfedge(v1) == h0) {
                    if (next1 == h0) {
                        PointCloud::mark_deleted(v1);
                    } else
                        Graph::set_halfedge(v1, next1);
                }
            }
        }

        // update outgoing halfedge handles of remaining vertices
        auto vit(vertices_.begin()), vend(vertices_.end());
        for (; vit != vend; ++vit) {
            adjust_outgoing_halfedge(*vit);
        }
    }

    size_t Mesh::get_valence(const Face &f) const {
        auto vv = get_vertices(f);
        return std::distance(vv.begin(), vv.end());
    }


    void Mesh::split(Face f, Vertex v) {
        // Split an arbitrary face into triangles by connecting each vertex of face
        // f to vertex v . Face f will remain valid (it will become one of the
        // triangles). The halfedge handles of the new triangles will point to the
        // old halfedges.

        Halfedge hend = get_halfedge(f);
        Halfedge h = get_next(hend);

        Halfedge hold = new_edge(get_vertex(hend), v);

        set_next(hend, hold);
        set_face(hold, f);

        hold = get_opposite(hold);

        while (h != hend) {
            Halfedge hnext = get_next(h);

            Face fnew = new_face();
            set_halfedge(fnew, h);

            Halfedge hnew = new_edge(get_vertex(h), v);

            set_next(hnew, hold);
            set_next(hold, h);
            set_next(h, hnew);

            set_face(hnew, fnew);
            set_face(hold, fnew);
            set_face(h, fnew);

            hold = get_opposite(hnew);

            h = hnext;
        }

        set_next(hold, hend);
        set_next(get_next(hend), hold);

        set_face(hold, f);

        Graph::set_halfedge(v, hold);
    }

    void Mesh::triangulate(const Face &f) {
        Halfedge h = get_halfedge(f);
        Vertex v0 = Graph::get_vertex(get_opposite(h));
        Halfedge nh = Graph::get_next(h);

        while (Graph::get_vertex(Graph::get_next(nh)) != v0) {
            Halfedge nnh(Graph::get_next(nh));

            Face new_f = new_face();
            set_halfedge(new_f, h);

            Halfedge new_h = Graph::new_edge(Graph::get_vertex(nh), v0);

            Graph::set_next(h, nh);
            Graph::set_next(nh, new_h);
            Graph::set_next(new_h, h);

            set_face(h, new_f);
            set_face(nh, new_f);
            set_face(new_h, new_f);

            h = Graph::get_opposite(new_h);
            nh = nnh;
        }
        set_halfedge(f, h); //the last face takes the handle _fh

        Graph::set_next(h, nh);
        Graph::set_next(Graph::get_next(nh), h);

        set_face(h, f);
    }

    void Mesh::triangulate() {
        for (const auto &f: faces) {
            if (get_valence(f) != 3) {
                triangulate(f);
            }
        }
    }
}
