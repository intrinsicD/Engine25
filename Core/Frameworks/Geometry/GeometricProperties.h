//
// Created by alex on 1/7/25.
//

#ifndef GEOMETRICPROPERTIES_H
#define GEOMETRICPROPERTIES_H

#include "Properties.h"
#include "Math.h"

namespace Bcg {
    constexpr size_t BCG_INVALID_IDX = std::numeric_limits<size_t>::max();

    class Handle {
    public:
        explicit Handle(size_t idx = BCG_INVALID_IDX) : m_idx(idx) {
        }

        [[nodiscard]] size_t idx() const { return m_idx; }

        [[nodiscard]] bool is_valid() const { return m_idx != BCG_INVALID_IDX; }

        void invalidate() { m_idx = BCG_INVALID_IDX; }

        auto operator<=>(const Handle &rhs) const = default;

        size_t m_idx;
    };

    class Vertex : public Handle {
    public:
        using Handle::Handle;
    };

    class Halfedge : public Handle {
    public:
        using Handle::Handle;
    };

    class Edge : public Handle {
    public:
        using Handle::Handle;
    };

    class Face : public Handle {
    public:
        using Handle::Handle;
    };

    class Tet : public Handle {
    public:
        using Handle::Handle;
    };

    class Voxel : public Handle {
    public:
        using Handle::Handle;
    };

    class Node : public Handle {
    public:
        using Handle::Handle;
    };

    inline std::ostream &operator<<(std::ostream &os, Vertex v) {
        return (os << 'v' << v.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Halfedge h) {
        return (os << 'h' << h.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Edge e) {
        return (os << 'e' << e.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Face f) {
        return (os << 'f' << f.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Tet t) {
        return (os << 't' << t.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Voxel v) {
        return (os << 'v' << v.idx());
    }

    inline std::ostream &operator<<(std::ostream &os, Node n) {
        return (os << 'n' << n.idx());
    }

    struct VertexConnectivity {
        Halfedge h; ///< outgoing halfedge

        [[nodiscard]] size_t size() const { return 1; }

        friend std::ostream &operator<<(std::ostream &os, const VertexConnectivity &vc) {
            os << "h: " << vc.h.idx();
            return os;
        }
    };

    struct HalfedgeConnectivity {
        Vertex v; ///< to_vertex
        Halfedge nh; ///< next halfedge
        Halfedge ph; ///< prev halfedge
        Face f; ///< indicent face

        [[nodiscard]] size_t size() const { return 4; }

        friend std::ostream &operator<<(std::ostream &os, const HalfedgeConnectivity &hc) {
            os << "v: " << hc.v.idx();
            os << " nh: " << hc.nh.idx();
            os << " ph: " << hc.ph.idx();
            os << " f: " << hc.f.idx();
            return os;
        }
    };

    struct FaceConnectivity {
        Halfedge h; ///< incident halfedge

        [[nodiscard]] size_t size() const { return 3; }

        friend std::ostream &operator<<(std::ostream &os, const FaceConnectivity &fc) {
            os << "h: " << fc.h.idx();
            return os;
        }
    };

    template<class T>
    class VertexProperty : public Property<T> {
    public:
        explicit VertexProperty() = default;

        explicit VertexProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Vertex v) {
            return Property<T>::operator[](v.idx());
        }

        typename Property<T>::const_reference operator[](Vertex v) const {
            return Property<T>::operator[](v.idx());
        }
    };

    template<class T>
    class HalfedgeProperty : public Property<T> {
    public:
        explicit HalfedgeProperty() = default;

        explicit HalfedgeProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Halfedge h) {
            return Property<T>::operator[](h.idx());
        }

        typename Property<T>::const_reference operator[](Halfedge h) const {
            return Property<T>::operator[](h.idx());
        }
    };

    template<class T>
    class EdgeProperty : public Property<T> {
    public:
        explicit EdgeProperty() = default;

        explicit EdgeProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Edge e) {
            return Property<T>::operator[](e.idx());
        }

        typename Property<T>::const_reference operator[](Edge e) const {
            return Property<T>::operator[](e.idx());
        }
    };

    template<class T>
    class FaceProperty : public Property<T> {
    public:
        explicit FaceProperty() = default;

        explicit FaceProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Face f) {
            return Property<T>::operator[](f.idx());
        }

        typename Property<T>::const_reference operator[](Face f) const {
            return Property<T>::operator[](f.idx());
        }
    };

    template<class T>
    class TetProperty : public Property<T> {
    public:
        explicit TetProperty() = default;

        explicit TetProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Tet t) {
            return Property<T>::operator[](t.idx());
        }

        typename Property<T>::const_reference operator[](Tet t) const {
            return Property<T>::operator[](t.idx());
        }
    };

    template<class T>
    class VoxelProperty : public Property<T> {
    public:
        explicit VoxelProperty() = default;

        explicit VoxelProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Voxel v) {
            return Property<T>::operator[](v.idx());
        }

        typename Property<T>::const_reference operator[](Voxel v) const {
            return Property<T>::operator[](v.idx());
        }
    };

    template<class T>
    class NodeProperty : public Property<T> {
    public:
        explicit NodeProperty() = default;

        explicit NodeProperty(Property<T> p) : Property<T>(p) {
        }

        typename Property<T>::reference operator[](Node n) {
            return Property<T>::operator[](n.idx());
        }

        typename Property<T>::const_reference operator[](Node n) const {
            return Property<T>::operator[](n.idx());
        }
    };

    template<class DataContainer, typename HandleType>
    class Iterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = HandleType;
        using reference = HandleType &;
        using pointer = HandleType *;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(HandleType handle = HandleType(), const DataContainer *m = nullptr) : m_handle(handle), m_data(m) {
            if (m_data && m_data->has_garbage()) {
                while (m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) {
                    ++m_handle.m_idx;
                }
            }
        }

        HandleType operator*() const { return m_handle; }

        auto operator<=>(const Iterator &rhs) const = default;

        Iterator &operator++() {
            ++m_handle.m_idx;
            assert(m_data);
            while (m_data->has_garbage() && m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) {
                ++m_handle.m_idx;
            }
            return *this;
        }

        Iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement iterator
        Iterator &operator--() {
            --m_handle.m_idx;
            assert(m_data);
            while (m_data->has_garbage() && m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) {
                --m_handle.m_idx;
            }
            return *this;
        }

        //! post-decrement iterator
        Iterator operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        HandleType m_handle;
        const DataContainer *m_data;
    };


    template<class DataContainer>
    class VertexAroundVertexCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = Vertex &;
        using pointer = Vertex *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        VertexAroundVertexCirculatorBase(const DataContainer *data = nullptr,
                                         Vertex v = Vertex())
            : m_data(data) {
            if (m_data)
                m_halfedge = m_data->get_halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundVertexCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundVertexCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        VertexAroundVertexCirculatorBase &operator++() {
            assert(m_data);
            m_halfedge = m_data->rotate_ccw(m_halfedge);
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        VertexAroundVertexCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        VertexAroundVertexCirculatorBase &operator--() {
            assert(m_data);
            m_halfedge = m_data->rotate_cw(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        VertexAroundVertexCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        Vertex get_next() const{
            return m_data->get_vertex(m_data->rotate_ccw(m_halfedge));
        }


        Vertex get_prev() const{
            return m_data->get_vertex(m_data->rotate_cw(m_halfedge));
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const {
            assert(m_data);
            return m_data->get_vertex(m_halfedge);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.is_valid(); }

        //! \return the current halfedge
        [[nodiscard]] Halfedge halfedge() const { return m_halfedge; }

        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculatorBase &begin() {
            m_is_active = !m_halfedge.is_valid();
            return *this;
        }

        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all outgoing halfedges of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, halfedges(Vertex)
    template<class DataContainer>
    class HalfedgeAroundVertexCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge &;
        using pointer = Halfedge *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        HalfedgeAroundVertexCirculatorBase(const DataContainer *data = nullptr,
                                           Vertex v = Vertex())
            : m_data(data) {
            if (m_data)
                m_halfedge = m_data->get_halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundVertexCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundVertexCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        HalfedgeAroundVertexCirculatorBase &operator++() {
            assert(m_data);
            m_halfedge = m_data->rotate_ccw(m_halfedge);
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundVertexCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculatorBase &operator--() {
            assert(m_data);
            m_halfedge = m_data->rotate_cw(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        Halfedge get_next() const{
            return m_data->rotate_ccw(m_halfedge);
        }


        Halfedge get_prev() const{
            return m_data->rotate_cw(m_halfedge);
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return m_halfedge; }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.is_valid(); }

        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculatorBase &begin() {
            m_is_active = !m_halfedge.is_valid();
            return *this;
        }

        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all edges incident to a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, edges(Vertex)
    template<class DataContainer>
    class EdgeAroundVertexCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Edge;
        using reference = Edge &;
        using pointer = Edge *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        EdgeAroundVertexCirculatorBase(const DataContainer *data = nullptr,
                                       Vertex v = Vertex())
            : m_data(data) {
            if (m_data)
                m_halfedge = m_data->get_halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const EdgeAroundVertexCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const EdgeAroundVertexCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        EdgeAroundVertexCirculatorBase &operator++() {
            assert(m_data);
            m_halfedge = m_data->rotate_ccw(m_halfedge);
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        EdgeAroundVertexCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        EdgeAroundVertexCirculatorBase &operator--() {
            assert(m_data);
            m_halfedge = m_data->rotate_cw(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        EdgeAroundVertexCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }


        Edge get_next() const{
            return m_data->rotate_ccw(m_halfedge);
        }


        Edge get_prev() const{
            return m_data->rotate_cw(m_halfedge);
        }

        //! get the halfedge the circulator refers to
        Edge operator*() const { return m_data->get_edge(m_halfedge); }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.is_valid(); }

        // helper for C++11 range-based for-loops
        EdgeAroundVertexCirculatorBase &begin() {
            m_is_active = !m_halfedge.is_valid();
            return *this;
        }

        // helper for C++11 range-based for-loops
        EdgeAroundVertexCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all incident faces of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, HalfedgeAroundVertexCirculator, faces(Vertex)
    template<class DataContainer>
    class FaceAroundVertexCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Face;
        using reference = Face &;
        using pointer = Face *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! construct with data and vertex (vertex should not be isolated!)
        FaceAroundVertexCirculatorBase(const DataContainer *m = nullptr,
                                       Vertex v = Vertex())
            : m_data(m) {
            if (m_data) {
                m_halfedge = m_data->get_halfedge(v);
                if (m_halfedge.is_valid() && m_data->is_boundary(m_halfedge))
                    operator++();
            }
        }

        //! are two circulators equal?
        bool operator==(const FaceAroundVertexCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const FaceAroundVertexCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        FaceAroundVertexCirculatorBase &operator++() {
            assert(m_data && m_halfedge.is_valid());
            do {
                m_halfedge = m_data->rotate_ccw(m_halfedge);
            } while (m_data->is_boundary(m_halfedge));
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        FaceAroundVertexCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        FaceAroundVertexCirculatorBase &operator--() {
            assert(m_data && m_halfedge.is_valid());
            do
                m_halfedge = m_data->rotate_cw(m_halfedge);
            while (m_data->is_boundary(m_halfedge));
            return *this;
        }

        //! post-decrement (rotate clockwise)
        FaceAroundVertexCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }


        Face get_next() const{
            return m_data->get_face(m_data->rotate_ccw(m_halfedge));
        }


        Face get_prev() const{
            return m_data->get_face(m_data->rotate_cw(m_halfedge));
        }

        //! get the face the circulator refers to
        Face operator*() const {
            assert(m_data && m_halfedge.is_valid());
            return m_data->get_face(m_halfedge);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.is_valid(); }

        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculatorBase &begin() {
            m_is_active = !m_halfedge.is_valid();
            return *this;
        }

        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through the vertices of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundFaceCirculator, vertices(Face)
    template<class DataContainer>
    class VertexAroundFaceCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = Vertex &;
        using pointer = Vertex *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        VertexAroundFaceCirculatorBase(const DataContainer *m = nullptr,
                                       Face f = Face())
            : m_data(m) {
            if (m_data)
                m_halfedge = m_data->get_halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundFaceCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundFaceCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        VertexAroundFaceCirculatorBase &operator++() {
            assert(m_data && m_halfedge.is_valid());
            m_halfedge = m_data->get_next(m_halfedge);
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        VertexAroundFaceCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotates clockwise)
        VertexAroundFaceCirculatorBase &operator--() {
            assert(m_data && m_halfedge.is_valid());
            m_halfedge = m_data->get_prev(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        VertexAroundFaceCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        Vertex get_next() const{
            return m_data->get_vertex(m_data->get_next(m_halfedge));
        }


        Vertex get_prev() const{
            return m_data->get_vertex(m_data->get_prev(m_halfedge));
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const {
            assert(m_data && m_halfedge.is_valid());
            return m_data->get_vertex(m_halfedge);
        }

        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculatorBase &begin() {
            m_is_active = false;
            return *this;
        }

        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all halfedges of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundFaceCirculator, halfedges(Face)
    template<class DataContainer>
    class HalfedgeAroundFaceCirculatorBase {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge &;
        using pointer = Halfedge *;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        HalfedgeAroundFaceCirculatorBase(const DataContainer *m = nullptr,
                                         Face f = Face())
            : m_data(m) {
            if (m_data)
                m_halfedge = m_data->get_halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundFaceCirculatorBase &rhs) const {
            assert(m_data);
            assert(m_data == rhs.m_data);
            return (m_is_active && (m_halfedge == rhs.m_halfedge));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundFaceCirculatorBase &rhs) const {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        HalfedgeAroundFaceCirculatorBase &operator++() {
            assert(m_data && m_halfedge.is_valid());
            m_halfedge = m_data->get_next(m_halfedge);
            m_is_active = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundFaceCirculatorBase operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotates clockwise)
        HalfedgeAroundFaceCirculatorBase &operator--() {
            assert(m_data && m_halfedge.is_valid());
            m_halfedge = m_data->get_prev(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        HalfedgeAroundFaceCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return m_halfedge; }

        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculatorBase &begin() {
            m_is_active = false;
            return *this;
        }

        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculatorBase &end() {
            m_is_active = true;
            return *this;
        }

    private:
        const DataContainer *m_data;
        Halfedge m_halfedge;
        bool m_is_active{true}; // helper for C++11 range-based for-loops
    };

    class Vertices : public PropertyContainer {
    public:
        using VertexIterator = Iterator<Vertices, Vertex>;

        VertexProperty<bool> v_deleted;
        size_t num_deleted;

        Vertices() : v_deleted(vertex_property<bool>("v:deleted", false)), num_deleted(0) {
        }

        VertexIterator begin() {
            return {Vertex(0), this};
        }

        VertexIterator end() {
            return {Vertex(size()), this};
        }

        VertexIterator begin() const {
            return {Vertex(0), this};
        }

        VertexIterator end() const {
            return {Vertex(size()), this};
        }

        [[nodiscard]] size_t n_vertices() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_vertices() == 0;
        }

        [[nodiscard]] bool is_valid(const Vertex &v) const {
            return v.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Vertex &v) const {
            return v_deleted[v];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            v_deleted = vertex_property<bool>("v:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        VertexProperty<T> add_vertex_property(const std::string &name,
                                              const T t = T()) {
            return VertexProperty<T>(add<T>(name, t));
        }

        template<class T>
        VertexProperty<T> get_vertex_property(const std::string &name) const {
            return VertexProperty<T>(get<T>(name));
        }

        template<class T>
        VertexProperty<T> vertex_property(const std::string &name, const T t = T()) {
            return VertexProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_vertex_property(VertexProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_vertex_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> vertex_properties() const {
            return properties();
        }
    };

    class Halfedges : public PropertyContainer {
    public:
        using HalfEdgeIterator = Iterator<Halfedges, Halfedge>;

        HalfedgeProperty<bool> h_deleted;
        size_t num_deleted;

        Halfedges() : h_deleted(halfedge_property<bool>("h:deleted", false)), num_deleted(0) {
        }

        HalfEdgeIterator begin() {
            return {Halfedge(0), this};
        }

        HalfEdgeIterator end() {
            return {Halfedge(size()), this};
        }

        HalfEdgeIterator begin() const {
            return {Halfedge(0), this};
        }

        HalfEdgeIterator end() const {
            return {Halfedge(size()), this};
        }


        [[nodiscard]] size_t n_halfedges() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_halfedges() == 0;
        }

        [[nodiscard]] bool is_valid(const Halfedge &h) const {
            return h.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Halfedge &h) const {
            return h_deleted[h];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            h_deleted = halfedge_property<bool>("h:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        HalfedgeProperty<T> add_halfedge_property(const std::string &name,
                                                  const T t = T()) {
            return HalfedgeProperty<T>(add<T>(name, t));
        }

        template<class T>
        HalfedgeProperty<T> get_halfedge_property(const std::string &name) const {
            return HalfedgeProperty<T>(get<T>(name));
        }

        template<class T>
        HalfedgeProperty<T> halfedge_property(const std::string &name, const T t = T()) {
            return HalfedgeProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_halfedge_property(HalfedgeProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_halfedge_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> halfedgeproperties() const {
            return properties();
        }
    };

    class Edges : public PropertyContainer {
    public:
        using EdgeIterator = Iterator<Edges, Edge>;

        EdgeProperty<bool> e_deleted;
        size_t num_deleted;

        Edges() : e_deleted(edge_property<bool>("e:deleted", false)), num_deleted(0) {
        }

        EdgeIterator begin() {
            return {Edge(0), this};
        }

        EdgeIterator end() {
            return {Edge(size()), this};
        }

        EdgeIterator begin() const {
            return {Edge(0), this};
        }

        EdgeIterator end() const {
            return {Edge(size()), this};
        }

        [[nodiscard]] size_t n_edges() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_edges() == 0;
        }

        [[nodiscard]] bool is_valid(const Edge &e) const {
            return e.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Edge &e) const {
            return e_deleted[e];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            e_deleted = edge_property<bool>("e:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        EdgeProperty<T> add_edge_property(const std::string &name,
                                          const T t = T()) {
            return EdgeProperty<T>(add<T>(name, t));
        }

        template<class T>
        EdgeProperty<T> get_edge_property(const std::string &name) const {
            return EdgeProperty<T>(get<T>(name));
        }

        template<class T>
        EdgeProperty<T> edge_property(const std::string &name, const T t = T()) {
            return EdgeProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_edge_property(EdgeProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_edge_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> edge_properties() const {
            return properties();
        }
    };

    class Faces : public PropertyContainer {
    public:
        using FaceIterator = Iterator<Faces, Face>;

        FaceProperty<bool> f_deleted;
        size_t num_deleted;

        Faces() : f_deleted(face_property<bool>("f:deleted", false)), num_deleted(0) {
        }

        FaceIterator begin() {
            return {Face(0), this};
        }

        FaceIterator end() {
            return {Face(size()), this};
        }

        FaceIterator begin() const {
            return {Face(0), this};
        }

        FaceIterator end() const {
            return {Face(size()), this};
        }

        [[nodiscard]] size_t n_faces() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_faces() == 0;
        }

        [[nodiscard]] bool is_valid(const Face &f) const {
            return f.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Face &f) const {
            return f_deleted[f];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            f_deleted = face_property<bool>("f:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        FaceProperty<T> add_face_property(const std::string &name,
                                          const T t = T()) {
            return FaceProperty<T>(add<T>(name, t));
        }

        template<class T>
        FaceProperty<T> get_face_property(const std::string &name) const {
            return FaceProperty<T>(get<T>(name));
        }

        template<class T>
        FaceProperty<T> face_property(const std::string &name, const T t = T()) {
            return FaceProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_face_property(FaceProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_face_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> face_properties() const {
            return properties();
        }
    };

    class Tets : public PropertyContainer {
    public:
        using TetIterator = Iterator<Tets, Tet>;

        TetProperty<bool> t_deleted;
        size_t num_deleted;

        Tets() : t_deleted(tet_property<bool>("t:deleted", false)), num_deleted(0) {
        }

        TetIterator begin() {
            return {Tet(0), this};
        }

        TetIterator end() {
            return {Tet(size()), this};
        }

        TetIterator begin() const {
            return {Tet(0), this};
        }

        TetIterator end() const {
            return {Tet(size()), this};
        }

        [[nodiscard]] size_t n_tets() const { return size() - num_deleted; }

        [[nodiscard]] bool is_valid(const Tet &t) const {
            return t.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Tet &t) const {
            return t_deleted[t];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            t_deleted = tet_property<bool>("t:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        TetProperty<T> add_tet_property(const std::string &name,
                                        const T t = T()) {
            return TetProperty<T>(add<T>(name, t));
        }

        template<class T>
        TetProperty<T> get_tet_property(const std::string &name) const {
            return TetProperty<T>(get<T>(name));
        }

        template<class T>
        TetProperty<T> tet_property(const std::string &name, const T t = T()) {
            return TetProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_tet_property(TetProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_tet_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> tet_properties() const {
            return properties();
        }
    };


    class Voxels : public PropertyContainer {
    public:
        using VoxelIterator = Iterator<Voxels, Voxel>;

        VoxelProperty<bool> v_deleted;
        size_t num_deleted;

        Voxels() : v_deleted(voxel_property<bool>("v:deleted", false)), num_deleted(0) {
        }

        VoxelIterator begin() {
            return {Voxel(0), this};
        }

        VoxelIterator end() {
            return {Voxel(size()), this};
        }

        VoxelIterator begin() const {
            return {Voxel(0), this};
        }

        VoxelIterator end() const {
            return {Voxel(size()), this};
        }

        [[nodiscard]] size_t n_voxels() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_voxels() == 0;
        }

        [[nodiscard]] bool is_valid(const Voxel &v) const {
            return v.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Voxel &v) const {
            return !v.is_valid() || v_deleted[v];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            v_deleted = voxel_property<bool>("v:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        VoxelProperty<T> add_voxel_property(const std::string &name,
                                            const T t = T()) {
            return VoxelProperty<T>(add<T>(name, t));
        }

        template<class T>
        VoxelProperty<T> get_voxel_property(const std::string &name) const {
            return VoxelProperty<T>(get<T>(name));
        }

        template<class T>
        VoxelProperty<T> voxel_property(const std::string &name, const T t = T()) {
            return VoxelProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_voxel_property(VoxelProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_voxel_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> voxel_properties() const {
            return properties();
        }
    };


    class Nodes : public PropertyContainer {
    public:
        using NodeIterator = Iterator<Nodes, Node>;

        NodeProperty<bool> n_deleted;
        size_t num_deleted;

        Nodes() : n_deleted(node_property<bool>("n:deleted", false)), num_deleted(0) {
        }

        NodeIterator begin() {
            return {Node(0), this};
        }

        NodeIterator end() {
            return {Node(size()), this};
        }

        NodeIterator begin() const {
            return {Node(0), this};
        }

        NodeIterator end() const {
            return {Node(size()), this};
        }

        [[nodiscard]] size_t n_nodes() const { return size() - num_deleted; }

        [[nodiscard]] bool is_empty() const override {
            return n_nodes() == 0;
        }

        [[nodiscard]] bool is_valid(const Node &v) const {
            return v.idx() < size();
        }

        [[nodiscard]] bool is_deleted(const Node &n) const {
            return !n.is_valid() || n_deleted[n];
        }

        [[nodiscard]] bool has_garbage() const {
            return num_deleted > 0;
        }

        void clear() override {
            PropertyContainer::clear();
            n_deleted = node_property<bool>("n:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        NodeProperty<T> add_node_property(const std::string &name,
                                            const T t = T()) {
            return NodeProperty<T>(add<T>(name, t));
        }

        template<class T>
        NodeProperty<T> get_node_property(const std::string &name) const {
            return NodeProperty<T>(get<T>(name));
        }

        template<class T>
        NodeProperty<T> node_property(const std::string &name, const T t = T()) {
            return NodeProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_node_property(NodeProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_node_property(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> node_properties() const {
            return properties();
        }
    };
}

namespace std {
    template <>
    struct hash<Bcg::Node> {
        size_t operator()(const Bcg::Node &handle) const noexcept {
            return std::hash<size_t>()(handle.idx());
        }
    };
}

#endif //GEOMETRICPROPERTIES_H
