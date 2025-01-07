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
        size_t idx() const { return m_idx; }

        bool is_valid() const { return m_idx != BCG_INVALID_IDX; }

        void invalidate() { m_idx = BCG_INVALID_IDX; }

        auto operator<=>(const Handle &rhs) const = default;

    protected:
        Handle(size_t idx) : m_idx(idx) {
        }

        size_t m_idx = BCG_INVALID_IDX;
    };

    class Vertex : public Handle {
        using Handle::Handle;
    };

    class Halfedge : public Handle {
        using Handle::Handle;
    };

    class Edge : public Handle {
        using Handle::Handle;
    };

    class Face : public Handle {
        using Handle::Handle;
    };

    class Tet : public Handle {
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

    template<class DataContainer, typename HandleType>
    class Iterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = HandleType;
        using reference = HandleType &;
        using pointer = HandleType *;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(HandleType handle = HandleType(), const DataContainer *m = nullptr) : handle_(handle), data_(m) {
            if (data_ && data_->has_garbage()) {
                while (data_->is_valid(handle_) && data_->is_deleted(handle_)) { ++handle_.idx_; }
            }
        }

        HandleType operator*() const { return handle_; }

        auto operator<=>(const Iterator &rhs) const = default;

        Iterator &operator++() {
            ++handle_.idx_;
            assert(data_);
            while (data_->has_garbage() && data_->is_valid(handle_) && data_->is_deleted(handle_)) {
                ++handle_.idx_;
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
            --handle_.idx_;
            assert(data_);
            while (data_->has_garbage() && data_->is_valid(handle_) && data_->is_deleted(handle_)) {
                --handle_.idx_;
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
        HandleType handle_;
        const DataContainer *data_;
    };

    template<class DataContainer, typename HandleType>
    class HandleContainer {
    public:
        HandleContainer(Iterator<DataContainer, HandleType> begin, Iterator<DataContainer, HandleType> end)
            : begin_(begin), end_(end) {
        }

        Iterator<DataContainer, HandleType> begin() const { return begin_; }

        Iterator<DataContainer, HandleType> end() const { return end_; }

    private:
        Iterator<DataContainer, HandleType> begin_;
        Iterator<DataContainer, HandleType> end_;
    };

    class Vertices : public PropertyContainer {
    public:
        using VertexIterator = Iterator<Vertices, Vertex>;

        VertexIterator begin() {
            return {Vertex(0), this};
        }

        VertexIterator end() {
            return {Vertex(size()), this};
        }
    };

    class Halfedges : public PropertyContainer {
    public:
        using HalfEdgeIterator = Iterator<Halfedges, Halfedge>;

        HalfEdgeIterator begin() {
            return {Halfedge(0), this};
        }

        HalfEdgeIterator end() {
            return {Halfedge(size()), this};
        }
    };

    class Edges : public PropertyContainer {
    public:
        using EdgeIterator = Iterator<Edges, Edge>;

        EdgeIterator begin() {
            return {Edge(0), this};
        }

        EdgeIterator end() {
            return {Edge(size()), this};
        }
    };

    class Faces : public PropertyContainer {
    public:
        using FaceIterator = Iterator<Faces, Face>;

        FaceIterator begin() {
            return {Face(0), this};
        }

        FaceIterator end() {
            return {Face(size()), this};
        }
    };

    class Tets : public PropertyContainer {
    public:
        using TetIterator = Iterator<Tets, Tet>;

        TetIterator begin() {
            return {Tet(0), this};
        }

        TetIterator end() {
            return {Tet(size()), this};
        }
    };
}

#endif //GEOMETRICPROPERTIES_H
