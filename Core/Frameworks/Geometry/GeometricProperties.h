//
// Created by alex on 1/7/25.
//

#ifndef GEOMETRICPROPERTIES_H
#define GEOMETRICPROPERTIES_H

#include "Properties.h"
#include "Math.h"
#include "source/compiler-core/slang-source-loc.h"

namespace Bcg {
    constexpr size_t BCG_INVALID_IDX = std::numeric_limits<size_t>::max();

    class Handle {
    public:
        explicit Handle(size_t idx = BCG_INVALID_IDX) : m_idx(idx) {

        }

        inline size_t idx() const { return m_idx; }

        inline bool is_valid() const { return m_idx != BCG_INVALID_IDX; }

        inline void invalidate() { m_idx = BCG_INVALID_IDX; }

        inline auto operator<=>(const Handle &rhs) const = default;

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


    template<class T>
    class VertexProperty : public Property<T> {
    public:
        explicit VertexProperty() = default;

        explicit VertexProperty(Property<T> p) : Property<T>(p) {
        }

        inline typename Property<T>::reference operator[](Vertex v) {
            return Property<T>::operator[](v.idx());
        }

        inline typename Property<T>::const_reference operator[](Vertex v) const {
            return Property<T>::operator[](v.idx());
        }
    };

    template<class T>
    class HalfedgeProperty : public Property<T> {
    public:
        explicit HalfedgeProperty() = default;

        explicit HalfedgeProperty(Property<T> p) : Property<T>(p) {
        }

        inline typename Property<T>::reference operator[](Halfedge h) {
            return Property<T>::operator[](h.idx());
        }

        inline typename Property<T>::const_reference operator[](Halfedge h) const {
            return Property<T>::operator[](h.idx());
        }
    };

    template<class T>
    class EdgeProperty : public Property<T> {
    public:
        explicit EdgeProperty() = default;

        explicit EdgeProperty(Property<T> p) : Property<T>(p) {
        }

        inline typename Property<T>::reference operator[](Edge e) {
            return Property<T>::operator[](e.idx());
        }

        inline typename Property<T>::const_reference operator[](Edge e) const {
            return Property<T>::operator[](e.idx());
        }
    };

    template<class T>
    class FaceProperty : public Property<T> {
    public:
        explicit FaceProperty() = default;

        explicit FaceProperty(Property<T> p) : Property<T>(p) {
        }

        inline typename Property<T>::reference operator[](Face f) {
            return Property<T>::operator[](f.idx());
        }

        inline typename Property<T>::const_reference operator[](Face f) const {
            return Property<T>::operator[](f.idx());
        }
    };

    template<class T>
    class TetProperty : public Property<T> {
    public:
        explicit TetProperty() = default;

        explicit TetProperty(Property<T> p) : Property<T>(p) {
        }

        inline typename Property<T>::reference operator[](Tet t) {
            return Property<T>::operator[](t.idx());
        }

        inline typename Property<T>::const_reference operator[](Tet t) const {
            return Property<T>::operator[](t.idx());
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
                while (m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) { ++m_handle.m_idx; }
            }
        }

        inline HandleType operator*() const { return m_handle; }

        inline auto operator<=>(const Iterator &rhs) const = default;

        Iterator &operator++() {
            ++m_handle.idx_;
            assert(m_data);
            while (m_data->has_garbage() && m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) {
                ++m_handle.idx_;
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
            --m_handle.idx_;
            assert(m_data);
            while (m_data->has_garbage() && m_data->is_valid(m_handle) && m_data->is_deleted(m_handle)) {
                --m_handle.idx_;
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
            m_halfedge = m_data->rotate_cw(m_halfedge);
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

        //! get the vertex the circulator refers to
        Vertex operator*() const {
            assert(m_data);
            return m_data->to_vertex(m_halfedge);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return m_halfedge.is_valid(); }

        //! \return the current halfedge
        Halfedge halfedge() const { return m_halfedge; }

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
            m_halfedge = m_data->crotate_cw(m_halfedge);
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

        //! get the halfedge the circulator refers to
        Edge operator*() const { return m_data->edge(m_halfedge); }

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
                m_halfedge = m_data->rotate_cw(m_halfedge);
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

        //! get the face the circulator refers to
        Face operator*() const {
            assert(m_data && m_halfedge.is_valid());
            return m_data->face(m_halfedge);
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
            m_halfedge = m_data->next_halfedge(m_halfedge);
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
            m_halfedge = m_data->prev_halfedge(m_halfedge);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        VertexAroundFaceCirculatorBase operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const {
            assert(m_data && m_halfedge.is_valid());
            return m_data->to_vertex(m_halfedge);
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
            m_halfedge = m_data->next_halfedge(m_halfedge);
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
            m_halfedge = m_data->prev_halfedge(m_halfedge);
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

        inline VertexIterator begin() {
            return {Vertex(0), this};
        }

        inline VertexIterator end() {
            return {Vertex(size()), this};
        }

        [[nodiscard]] inline size_t n_vertices() const { return size() - num_deleted; }

        [[nodiscard]] inline bool is_empty() const {
            return n_vertices() == 0;
        }

        [[nodiscard]] inline bool is_valid(const Vertex &v) const {
            return v.idx() < size();
        }

        [[nodiscard]] inline bool is_deleted(const Vertex &v) const {
            return v_deleted[v];
        }

        [[nodiscard]] inline bool has_garbage() const {
            return num_deleted > 0;
        }

        [[nodiscard]] inline size_t num_v_deleted() const {
            return num_deleted;
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

        Vertex new_vertex() {
            push_back();
            return Vertex(size() - 1);
        }
    };

    class Halfedges : public PropertyContainer {
    public:
        using HalfEdgeIterator = Iterator<Halfedges, Halfedge>;

        HalfedgeProperty<bool> h_deleted;
        size_t num_deleted = 0;

        Halfedges() : h_deleted(m_halfedgeproperty<bool>("h:deleted", false)), num_deleted(0) {

        }

        inline HalfEdgeIterator begin() {
            return {Halfedge(0), this};
        }

        inline HalfEdgeIterator end() {
            return {Halfedge(size()), this};
        }


        [[nodiscard]] inline size_t n_halfedges() const { return size() - num_deleted; }

        [[nodiscard]] inline bool is_empty() const {
            return n_halfedges() == 0;
        }

        [[nodiscard]] inline bool is_valid(const Halfedge &h) const {
            return h.idx() < size();
        }

        [[nodiscard]] inline bool is_deleted(const Halfedge &h) const {
            return h_deleted[h];
        }

        [[nodiscard]] inline bool has_garbage() const {
            return num_deleted > 0;
        }

        [[nodiscard]] inline size_t num_h_deleted() const {
            return num_deleted;
        }

        void clear() override {
            PropertyContainer::clear();
            h_deleted = m_halfedgeproperty<bool>("h:deleted", false);
            num_deleted = 0;
        }

        template<class T>
        HalfedgeProperty<T> add_m_halfedgeproperty(const std::string &name,
                                                   const T t = T()) {
            return HalfedgeProperty<T>(add<T>(name, t));
        }

        template<class T>
        HalfedgeProperty<T> get_m_halfedgeproperty(const std::string &name) const {
            return HalfedgeProperty<T>(get<T>(name));
        }

        template<class T>
        HalfedgeProperty<T> m_halfedgeproperty(const std::string &name, const T t = T()) {
            return HalfedgeProperty<T>(get_or_add<T>(name, t));
        }

        template<class T>
        void remove_m_halfedgeproperty(HalfedgeProperty<T> &p) {
            remove(p);
        }

        [[nodiscard]] bool has_m_halfedgeproperty(const std::string &name) const {
            return exists(name);
        }

        [[nodiscard]] std::vector<std::string> m_halfedgeproperties() const {
            return properties();
        }
    };

    class Edges : public PropertyContainer {
    public:
        using EdgeIterator = Iterator<Edges, Edge>;

        EdgeProperty<bool> e_deleted;
        size_t num_deleted = 0;

        Edges() : e_deleted(edge_property<bool>("e:deleted", false)), num_deleted(0) {

        }

        inline EdgeIterator begin() {
            return {Edge(0), this};
        }

        inline EdgeIterator end() {
            return {Edge(size()), this};
        }

        [[nodiscard]] inline size_t n_edges() const { return size() - num_deleted; }

        [[nodiscard]] inline bool is_empty() const {
            return n_edges() == 0;
        }

        [[nodiscard]] inline bool is_valid(const Edge &e) const {
            return e.idx() < size();
        }

        [[nodiscard]] inline bool is_deleted(const Edge &e) const {
            return e_deleted[e];
        }

        [[nodiscard]] inline bool has_garbage() const {
            return num_deleted > 0;
        }

        [[nodiscard]] inline size_t num_e_deleted() const {
            return num_deleted;
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

        FaceProperty<bool> deleted_faces;

        Faces() : deleted_faces(get_or_add<bool>("f:deleted", false)) {

        }

        inline FaceIterator begin() {
            return {Face(0), this};
        }

        inline FaceIterator end() {
            return {Face(size()), this};
        }

        [[nodiscard]] inline bool is_valid(const Face &f) const {
            return f.idx() < size();
        }

        [[nodiscard]] inline bool is_deleted(const Face &f) const {
            return deleted_faces && deleted_faces[f];
        }

        [[nodiscard]] inline bool has_garbage() const {
            return num_deleted > 0;
        }

        [[nodiscard]] inline size_t num_deleted_faces() const {
            return num_deleted;
        }

    protected:
        size_t num_deleted = 0;
    };

    class Tets : public PropertyContainer {
    public:
        using TetIterator = Iterator<Tets, Tet>;

        TetProperty<bool> deleted_tets;

        Tets() : deleted_tets(get_or_add<bool>("t:deleted", false)) {

        }

        inline TetIterator begin() {
            return {Tet(0), this};
        }

        inline TetIterator end() {
            return {Tet(size()), this};
        }

        [[nodiscard]] inline bool is_valid(const Tet &t) const {
            return t.idx() < size();
        }

        [[nodiscard]] inline bool is_deleted(const Tet &t) const {
            return deleted_tets && deleted_tets[t];
        }

        [[nodiscard]] inline bool has_garbage() const {
            return num_deleted > 0;
        }

        [[nodiscard]] inline size_t num_deleted_tets() const {
            return num_deleted;
        }

    protected:
        size_t num_deleted = 0;
    };
}

#endif //GEOMETRICPROPERTIES_H
