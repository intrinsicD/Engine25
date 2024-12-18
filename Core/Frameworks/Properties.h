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
        virtual void Reserve(size_t n) = 0;

        //! Resize storage to hold n elements.
        virtual void Resize(size_t n) = 0;

        //! Free unused memory.
        virtual void FreeMemory() = 0;

        //! Extend the number of elements by one.
        virtual void PushBack() = 0;

        //! Let two elements swap their storage place.
        virtual void Swap(size_t i0, size_t i1) = 0;

        //! Return a deep copy of self.
        [[nodiscard]] virtual BasePropertyArray *Clone() const = 0;

        //! Return the name of the property
        [[nodiscard]] virtual const std::string &Name() const = 0;

        [[nodiscard]] virtual size_t Size() const = 0;

        [[nodiscard]] virtual size_t Dims() const = 0;
    };

    template<typename S, typename Enable = void>
    size_t GetDimensions(const S &) {
        return 1;
    }

    template<typename S>
    std::enable_if_t<std::is_member_function_pointer_v<decltype(&S::size)>, size_t>
    GetDimensions(const S &s) {
        return s.size();
    }

    template<class T>
    class PropertyArray final : public BasePropertyArray {
    public:
        using ValueType = T;
        using VectorType = std::vector<ValueType>;
        using reference = typename VectorType::reference;
        using const_reference = typename VectorType::const_reference;

        explicit PropertyArray(std::string name, T t = T())
            : name_(std::move(name)), value_(std::move(t)) {
        }

        void Reserve(size_t n) override { data_.reserve(n); }

        void Resize(size_t n) override { data_.resize(n, value_); }

        void PushBack() override { data_.push_back(value_); }

        void FreeMemory() override { data_.shrink_to_fit(); }

        void Swap(size_t i0, size_t i1) override {
            T d(data_[i0]);
            data_[i0] = data_[i1];
            data_[i1] = d;
        }

        [[nodiscard]] BasePropertyArray *Clone() const override {
            auto *p = new PropertyArray<T>(name_, value_);
            p->data_ = data_;
            return p;
        }

        //! Get pointer to array (does not work for T==bool)
        [[nodiscard]] const T *Data() const { return &data_[0]; }

        //! Get reference to the underlying vector
        std::vector<T> &Vector() { return data_; }

        //! Access the i'th element. No range check is performed!
        reference operator[](size_t idx) {
            assert(idx < data_.size());
            return data_[idx];
        }

        //! Const access to the i'th element. No range check is performed!
        const_reference operator[](size_t idx) const {
            assert(idx < data_.size());
            return data_[idx];
        }

        //! Return the name of the property
        [[nodiscard]] const std::string &Name() const override { return name_; }


        [[nodiscard]] size_t Size() const override {
            return data_.size();
        }

        [[nodiscard]] size_t Dims() const override {
            return GetDimensions(value_);
        }

    private:
        std::string name_;
        VectorType data_;
        ValueType value_;
    };

    // specialization for bool properties
    template<>
    inline const bool *PropertyArray<bool>::Data() const {
        assert(false);
        return nullptr;
    }

    template<class T>
    class Property {
    public:
        using reference = typename PropertyArray<T>::reference;
        using const_reference = typename PropertyArray<T>::const_reference;

        friend class PropertyContainer;

        friend class SurfaceMesh;

        friend class Graph;

        friend class PointCloud;

        explicit Property(PropertyArray<T> *p = nullptr) : parray_(p) {
        }

        void reset() { parray_ = nullptr; }

        explicit operator bool() const { return parray_ != nullptr; }

        [[nodiscard]] const std::string &Name() const { return parray_->Name(); }

        reference operator[](size_t i) {
            assert(parray_ != nullptr);
            return (*parray_)[i];
        }

        const_reference operator[](size_t i) const {
            assert(parray_ != nullptr);
            return (*parray_)[i];
        }

        const T *Data() const {
            assert(parray_ != nullptr);
            return parray_->Data();
        }

        std::vector<T> &Vector() {
            assert(parray_ != nullptr);
            return parray_->Vector();
        }

        [[nodiscard]] const BasePropertyArray *Base() const {
            return parray_;
        }

    private:
        PropertyArray<T> &Array() {
            assert(parray_ != nullptr);
            return *parray_;
        }

        const PropertyArray<T> &Array() const {
            assert(parray_ != nullptr);
            return *parray_;
        }

        PropertyArray<T> *parray_;
    };

    class PropertyContainer {
    public:
        // default constructor
        PropertyContainer() = default;

        // destructor (deletes all property arrays)
        virtual ~PropertyContainer() { Clear(); }

        // copy constructor: performs deep copy of property arrays
        PropertyContainer(const PropertyContainer &rhs) { operator=(rhs); }

        // assignment: performs deep copy of property arrays
        PropertyContainer &operator=(const PropertyContainer &rhs) {
            if (this != &rhs) {
                Clear();
                size_ = rhs.size_;
                for(const auto &pair: rhs.parrays_) {
                    parrays_[pair.first] = pair.second->Clone();
                }
            }
            return *this;
        }

        [[nodiscard]] bool IsEmpty() const { return size_ == 0; }

        // returns the current size of the property arrays
        [[nodiscard]] size_t Size() const { return size_; }

        // returns the number of property arrays
        [[nodiscard]] size_t NumProperties() const { return parrays_.size(); }

        // returns a vector of all property names
        [[nodiscard]] std::vector<std::string> Properties(std::initializer_list<int> filter_dims = {}) const {
            //TODO figure out filtering by type, float, int, other custom types ...
            std::vector<std::string> names;
            names.reserve(parrays_.size());
            for (const auto &parray: parrays_) {
                if (filter_dims.size() > 0) {
                    for (const auto &dim: filter_dims) {
                        if (parray.second->Dims() == dim) {
                            names.emplace_back(parray.first);
                        }
                    }
                } else {
                    names.emplace_back(parray.first);
                }
            }
            return names;
        }

        // add a property with name \p name and default value \p t
        template<class T>
        Property<T> Add(const std::string &name, const T t = T()) {
            // throw exception if a property with this name already exists
            if (parrays_.contains(name)) {
                LOG_WARN(fmt::format("[PropertyContainer] A property with name \"{}\" already exists.", name));
                return Property<T>();
            }

            // otherwise add the property
            auto *p = new PropertyArray<T>(name, t);
            p->Resize(size_);
            parrays_[name] = p;
            return Property<T>(p);
        }

        // do we have a property with a given name?
        [[nodiscard]] bool Exists(const std::string &name) const {
            return parrays_.contains(name);
        }

        // get a property by its name. returns invalid property if it does not exist.
        template<class T>
        Property<T> Get(const std::string &name) const {
            auto iter = parrays_.find(name);
            if (iter != parrays_.end()) {
                return Property<T>(dynamic_cast<PropertyArray<T> *>(iter->second));
            }
            return Property<T>();
        }

        [[nodiscard]] BasePropertyArray *GetBase(const std::string &name) const {
            auto iter = parrays_.find(name);
            if (iter != parrays_.end()) {
                return iter->second;
            }
            return nullptr;
        }

        // returns a property if it exists, otherwise it creates it first.
        template<class T>
        Property<T> GetOrAdd(const std::string &name, const T t = T()) {
            Property<T> p = Get<T>(name);
            if (!p) {
                p = Add<T>(name, t);
            }
            return p;
        }

        // delete a property
        template<class T>
        void Remove(Property<T> &h) {
            const auto end = parrays_.end();
            for (auto it = parrays_.begin(); it != end; ++it) {
                if (*it == h.parray_) {
                    delete it->second;
                    parrays_.erase(it);
                    h.reset();
                    break;
                }
            }
        }

        // delete all properties
        void Clear() {
            for (auto &parray: parrays_) {
                delete parray.second;
            }
            parrays_.clear();
            size_ = 0;
        }

        // reserve memory for n entries in all arrays
        void Reserve(size_t n) const {
            for (auto &parray: parrays_) {
                parray.second->Reserve(n);
            }
        }

        // resize all arrays to size n
        void Resize(size_t n) {
            for (auto &parray: parrays_) {
                parray.second->Resize(n);
            }
            size_ = n;
        }

        // free unused space in all arrays
        void FreeMemory() const {
            for (auto &parray: parrays_) {
                parray.second->FreeMemory();
            }
        }

        // add a new element to each vector
        void PushBack() {
            for (auto &parray: parrays_) {
                parray.second->PushBack();
            }
            ++size_;
        }

        // swap elements i0 and i1 in all arrays
        void Swap(size_t i0, size_t i1) const {
            for (auto &parray: parrays_) {
                parray.second->Swap(i0, i1);
            }
        }

        [[nodiscard]] const std::unordered_map<std::string, BasePropertyArray *> &GetArray() const {
            return parrays_;
        }

    private:
        std::unordered_map<std::string, BasePropertyArray *> parrays_;
        //std::vector<BasePropertyArray *> parrays_;
        size_t size_{0};
    };
}

#endif //ENGINE25_PROPERTIES_H
