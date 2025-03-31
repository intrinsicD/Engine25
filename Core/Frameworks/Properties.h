// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#ifndef ENGINE25_PROPERTIES_H
#define ENGINE25_PROPERTIES_H

#include <algorithm>
#include <cassert>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include "Logger.h"

namespace Bcg {
    class BasePropertyArray {
    public:
        //! Destructor.
        virtual ~BasePropertyArray() = default;

        //! Reserve memory for n elements.
        virtual void reserve(size_t n) = 0;

        //! Resize storage to hold n elements.
        virtual void resize(size_t n) = 0;

        //! Free unused memory.
        virtual void free_memory() = 0;

        //! Extend the number of elements by one.
        virtual void push_back() = 0;

        //! Let two elements swap their storage place.
        virtual void swap(size_t i0, size_t i1) = 0;

        //! Return a deep copy of self.
        [[nodiscard]] virtual BasePropertyArray *clone() const = 0;

        //! Return the name of the property
        [[nodiscard]] virtual const std::string &name() const = 0;

        [[nodiscard]] virtual size_t size() const = 0;

        [[nodiscard]] virtual size_t dims() const = 0;

        [[nodiscard]] virtual void clear() = 0;

    private:
        friend class PropertyContainer;
    };

    template<typename S, typename Enable = void>
    size_t GetDimensions(const S &) {
        return 1;
    }

    template<typename S>
    std::enable_if_t<std::is_member_function_pointer_v<decltype(&S::size)> &&
                     !std::is_integral_v<decltype(std::declval<S>().size())>, size_t>
    GetDimensions(const S &s) {
        return s.size(); // Specialized for types with a `size` member function that does not return integral types
    }

    template<class T>
    class PropertyArray final : public BasePropertyArray {
    public:
        using ValueType = T;
        using VectorType = std::vector<ValueType>;
        using reference = typename VectorType::reference;
        using const_reference = typename VectorType::const_reference;

        explicit PropertyArray(std::string name, T t = T())
            : m_name(std::move(name)), m_value(std::move(t)) {
        }

        void reserve(size_t n) override { m_data.reserve(n); }

        void resize(size_t n) override { m_data.resize(n, m_value); }

        void push_back() override { m_data.push_back(m_value); }

        void free_memory() override { m_data.shrink_to_fit(); }

        void swap(size_t i0, size_t i1) override {
            T d(m_data[i0]);
            m_data[i0] = m_data[i1];
            m_data[i1] = d;
        }

        //! Get pointer to array (does not work for T==bool)
        [[nodiscard]] T *data() { return &m_data[0]; }

        [[nodiscard]] const T *data() const { return &m_data[0]; }

        //! Get reference to the underlying vector
        std::vector<T> &vector() { return m_data; }

        [[nodiscard]] const std::vector<T> &vector() const { return m_data; }

        //! Access the i'th element. No range check is performed!
        reference operator[](size_t idx) {
            assert(idx < m_data.size()); //TODO this fails frequently... and i dont know why!?
            return m_data[idx];
        }

        //! Const access to the i'th element. No range check is performed!
        const_reference operator[](size_t idx) const {
            assert(idx < m_data.size());
            return m_data[idx];
        }

        //! Return the name of the property
        [[nodiscard]] const std::string &name() const override { return m_name; }

        [[nodiscard]] size_t size() const override { return m_data.size(); }

        [[nodiscard]] size_t dims() const override { return GetDimensions(m_value); }

        [[nodiscard]] void clear() override {
            resize(0);
        }

        [[nodiscard]] BasePropertyArray *clone() const override {
            auto *p = new PropertyArray<T>(m_name, m_value);
            p->m_data = m_data;
            return p;
        }

    private:
        std::string m_name;
        VectorType m_data;
        ValueType m_value;
    };

    // specialization for bool properties
    template<>
    inline const bool *PropertyArray<bool>::data() const {
        assert(false);
        return nullptr;
    }

    template<class T>
    class Property {
    public:
        using reference = typename PropertyArray<T>::reference;
        using const_reference = typename PropertyArray<T>::const_reference;

        friend class PropertyContainer;

        friend class Mesh;

        friend class Graph;

        friend class PointCloud;

        explicit Property(PropertyArray<T> *p = nullptr) : m_parray(p) {
        }

        void reset() { m_parray = nullptr; }

        explicit operator bool() const { return m_parray != nullptr; }

        [[nodiscard]] const std::string &name() const { return m_parray->name(); }

        reference operator[](size_t i) {
            assert(m_parray != nullptr);
            return (*m_parray)[i];
        }

        const_reference operator[](size_t i) const {
            assert(m_parray != nullptr);
            return (*m_parray)[i];
        }

        const T *data() const {
            assert(m_parray != nullptr);
            return m_parray->data();
        }

        std::vector<T> &vector() {
            assert(m_parray != nullptr);
            return m_parray->vector();
        }

        const std::vector<T> &vector() const {
            assert(m_parray != nullptr);
            return m_parray->vector();
        }

        [[nodiscard]] BasePropertyArray *base() { return m_parray; }

        [[nodiscard]] const BasePropertyArray *base() const { return m_parray; }

        [[nodiscard]] const size_t size() const { return m_parray->size(); }

        [[nodiscard]] const size_t dims() const { return m_parray->dims(); }

    private:
        PropertyArray<T> *m_parray;
    };

    class PropertyContainer {
    public:
        // default constructor
        PropertyContainer() = default;

        // destructor (deletes all property arrays)
        virtual ~PropertyContainer() { clear(); }

        // copy constructor: performs deep copy of property arrays
        PropertyContainer(const PropertyContainer &rhs) { operator=(rhs); }

        // assignment: performs deep copy of property arrays
        PropertyContainer &operator=(const PropertyContainer &rhs) {
            if (this != &rhs) {
                clear();
                m_size = rhs.m_size;
                for (const auto &pair: rhs.m_parrays) {
                    m_parrays[pair.first] = pair.second->clone();
                }
            }
            return *this;
        }

        [[nodiscard]] virtual bool is_empty() const { return m_size == 0; }

        // returns the current size of the property arrays
        [[nodiscard]] size_t size() const { return m_size; }

        // returns the number of property arrays
        [[nodiscard]] size_t num_props() const { return m_parrays.size(); }

        // returns a vector of all property names
        [[nodiscard]] std::vector<std::string> properties(std::initializer_list<int> filter_dims = {}) const {
            //TODO figure out filtering by type, float, int, other custom types ...
            std::vector<std::string> names;
            names.reserve(m_parrays.size());
            for (const auto &parray: m_parrays) {
                if (filter_dims.size() > 0) {
                    for (const auto &dim: filter_dims) {
                        if (parray.second->dims() == dim) {
                            names.emplace_back(parray.first);
                        }
                    }
                } else {
                    names.emplace_back(parray.first);
                }
            }
            return names;
        }

        void link(const std::string &name, BasePropertyArray *parray) {
            // throw exception if a property with this name already exists
            if (m_parrays.contains(name)) {
                LOG_WARN(fmt::format("[PropertyContainer] A property with name \"{}\" already exists.", name));
                return;
            }

            // otherwise add the property
            m_parrays[name] = parray;
        }

        // add a property with name \p name and default value \p t
        template<class T>
        Property<T> add(const std::string &name, const T t = T()) {
            // throw exception if a property with this name already exists
            if (m_parrays.contains(name)) {
                LOG_WARN(fmt::format("[PropertyContainer] A property with name \"{}\" already exists.", name));
                return Property<T>();
            }

            // otherwise add the property
            auto *p = new PropertyArray<T>(name, t);
            p->resize(m_size);
            m_parrays[name] = p;
            return Property<T>(p);
        }

        // do we have a property with a given name?
        [[nodiscard]] bool exists(const std::string &name) const {
            return m_parrays.contains(name);
        }

        // get a property by its name. returns invalid property if it does not exist.
        template<class T>
        Property<T> get(const std::string &name) const {
            auto iter = m_parrays.find(name);
            if (iter != m_parrays.end()) {
                return Property<T>(dynamic_cast<PropertyArray<T> *>(iter->second));
            }
            return Property<T>();
        }

        [[nodiscard]] BasePropertyArray *get_base(const std::string &name) const {
            auto iter = m_parrays.find(name);
            if (iter != m_parrays.end()) {
                return iter->second;
            }
            return nullptr;
        }

        // returns a property if it exists, otherwise it creates it first.
        template<class T>
        Property<T> get_or_add(const std::string &name, const T t = T()) {
            Property<T> p = get<T>(name);
            if (!p) {
                p = add<T>(name, t);
            }
            return p;
        }

        // delete a property
        template<class T>
        void remove(Property<T> &h) {
            const auto end = m_parrays.end();
            for (auto it = m_parrays.begin(); it != end; ++it) {
                if (it->second == h.m_parray) {
                    delete it->second;
                    m_parrays.erase(it);
                    h.reset();
                    break;
                }
            }
        }

        // delete all properties
        virtual void clear() {
            for (auto &parray: m_parrays) {
                delete parray.second;
            }
            m_parrays.clear();
            m_size = 0;
        }

        // reserve memory for n entries in all arrays
        void reserve(size_t n) const {
            for (auto &parray: m_parrays) {
                parray.second->reserve(n);
            }
        }

        // resize all arrays to size n
        void resize(size_t n) {
            for (auto &parray: m_parrays) {
                parray.second->resize(n);
            }
            m_size = n;
        }

        // free unused space in all arrays
        void free_memory() const {
            for (auto &parray: m_parrays) {
                parray.second->free_memory();
            }
        }

        // add a new element to each vector
        void push_back() {
            for (auto &parray: m_parrays) {
                parray.second->push_back();
            }
            ++m_size;
        }

        // swap elements i0 and i1 in all arrays
        void swap(size_t i0, size_t i1) const {
            for (auto &parray: m_parrays) {
                parray.second->swap(i0, i1);
            }
        }

        [[nodiscard]] const std::unordered_map<std::string, BasePropertyArray *> &get_array() const {
            return m_parrays;
        }

    private:
        std::unordered_map<std::string, BasePropertyArray *> m_parrays;
        size_t m_size{0};
    };
}

#endif //ENGINE25_PROPERTIES_H
