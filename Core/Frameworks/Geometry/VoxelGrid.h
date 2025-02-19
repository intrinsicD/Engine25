//
// Created by alex on 1/17/25.
//

#ifndef VOXELGRID_H
#define VOXELGRID_H

#include "GeometricProperties.h"
#include "AABB.h"

namespace Bcg {
    /**
     * @brief Computes the sizes of the voxels in each dimension.
     *
     * This function calculates the sizes of the voxels in each dimension based on the axis-aligned bounding box (AABB)
     * and the grid dimensions. The voxel sizes are computed by dividing the size of the AABB by the grid dimensions.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param aabb The axis-aligned bounding box defining the bounds of the grid.
     * @param grid_dims The dimensions of the grid.
     * @return A vector containing the sizes of the voxels in each dimension.
     */
    template<typename T, int N>
    inline Vector<T, N> VoxelSizes(const AABB<T, N> &aabb, const Vector<int, N> &grid_dims) {
        return (aabb.max() - aabb.min()).array() / grid_dims.array().template cast<T>();
    }

    /**
     * @brief Computes the grid dimensions based on the AABB and voxel size.
     *
     * This function calculates the grid dimensions by dividing the size of the AABB by the voxel size.
     * The result is rounded up to ensure that the entire AABB is covered by the grid.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param aabb The axis-aligned bounding box defining the bounds of the grid.
     * @param voxel_size The size of the voxels.
     * @return A vector containing the dimensions of the grid.
     */
    template<typename T, int N>
    inline Vector<int, N> GridDims(const AABB<T, N> &aabb, const Vector<T, N> &voxel_size) {
        return ((aabb.max() - aabb.min()).array() / voxel_size.array()).ceil().template cast<int>();
    }

    /**
     * @brief Computes the strides for accessing the grid.
     *
     * This function calculates the strides for accessing the grid based on the grid dimensions.
     * The strides are used to convert multi-dimensional indices to a linear index.
     *
     * @tparam N The number of dimensions.
     * @param grid_dims The dimensions of the grid.
     * @return A vector containing the strides for each dimension.
     */
    template<int N>
    inline Vector<int, N> Strides(const Vector<int, N> &grid_dims) {
        Vector<int, N> strides = Vector<int, N>::Ones();
        if (grid_dims.size() > 1) {
            strides.tail(N - 1) = CumulativeProduct<int, N - 1>(grid_dims.head(N - 1));
        }
        return strides;
    }

    /**
     * @brief Computes the voxel index for a given point.
     *
     * This function calculates the voxel index for a given point based on the voxel size.
     * The point coordinates are divided by the voxel size to determine the index, and a small
     * epsilon value is added to ensure correct rounding.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param point The coordinates of the point.
     * @param voxel_size The size of the voxels.
     * @return A vector containing the voxel index in each dimension.
     */
    template<typename T, int N>
    inline Vector<int, N> VoxelIndex(const Vector<T, N> &point, const Vector<T, N> &voxel_size) {
        if (voxel_size.minCoeff() <= 0) {
            return Vector<int, N>::Zero();
        }
        return (point.array() / voxel_size.array() + T(1e-6)).template cast<int>();
    }

    /**
     * @brief Computes the linear index of a voxel.
     *
     * This function calculates the linear index of a voxel based on its multi-dimensional index
     * and the strides of the grid.
     *
     * @tparam N The number of dimensions.
     * @param idx The multi-dimensional index of the voxel.
     * @param strides The strides of the grid.
     * @return The linear index of the voxel.
     */
    template<int N>
    inline size_t VoxelLinearIndex(const Vector<int, N> &idx, const Vector<int, N> &strides) {
        return idx.dot(strides);
    }

    /**
     * @brief Computes the multi-dimensional index of a voxel.
     *
     * This function calculates the multi-dimensional index of a voxel based on its linear index
     * and the strides of the grid.
     *
     * @tparam N The number of dimensions.
     * @param linear_index The linear index of the voxel.
     * @param strides The strides of the grid.
     * @return The multi-dimensional index of the voxel.
     */
    template<int N>
    inline Vector<int, N> VoxelIndex(size_t linear_index, const Vector<int, N> &strides) {
        Vector<int, N> idx;
        for (int i = N - 1; i >= 0; --i) {
            idx[i] = linear_index / strides[i];
            linear_index -= idx[i] * strides[i];
        }
        return idx;
    }

    class VoxelGrid {
    public:
        std::unordered_map<size_t, Voxel> sparse_voxels_map;
        VoxelProperty<bool> v_deleted; ///< Deletion flag for voxels.
        VoxelProperty<size_t> v_linear_index; ///< Linear index for voxels.
        VoxelContainer voxels;

        /**
         * @brief Default constructor. Initializes properties and containers.
         */
        VoxelGrid();

        /**
         * @brief Virtual destructor for extensibility.
         */
        virtual ~VoxelGrid() = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another VoxelGrid instance to copy.
         */
        VoxelGrid(const VoxelGrid &rhs) : sparse_voxels_map() { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another VoxelGrid instance to copy.
         * @return Reference to this VoxelGrid.
         */
        VoxelGrid &operator=(const VoxelGrid &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another VoxelGrid instance to copy.
         * @return Reference to this VoxelGrid.
         */
        VoxelGrid &assign(const VoxelGrid &rhs);

        /**
         * @brief Clears all vertices and properties.
         */
        virtual void clear();

        /**
         * @brief Releases unused memory.
         */
        virtual void free_memory();

        /**
         * @brief Reserves space for a specified number of vertices.
         * @param nvoxels Number of voxels to reserve space for.
         */
        virtual void reserve(size_t nvoxels);

        /**
         * @brief Removes deleted voxels and compacts the data.
         */
        virtual void garbage_collection();

        // -------------------------------------------------------------------------------------------------------------
        // Voxel Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Returns the total number of undeleted voxels.
         * @return Number of undeleted voxels.
         */
        [[nodiscard]] size_t n_voxels() const { return voxels.n_voxels(); }

        /**
         * @brief Checks if the VoxelGrid is empty.
         * @return True if empty, false otherwise.
         */
        [[nodiscard]] bool is_empty() const { return n_voxels() == 0; }

        /**
         * @brief Checks if a voxel is deleted.
         * @param v Voxel to check.
         * @return True if deleted, false otherwise.
         */
        [[nodiscard]] bool is_deleted(const Voxel &v) const { return voxels.is_deleted(v); }

        /**
         * @brief Checks if a voxel is valid.
         * @param v Voxel to check.
         * @return True if valid, false otherwise.
         */
        [[nodiscard]] bool is_valid(const Voxel &v) const { return voxels.is_valid(v); }

        /**
         * @brief Checks if there is garbage (deleted voxels).
         * @return True if garbage exists, false otherwise.
         */
        [[nodiscard]] virtual bool has_garbage() const { return voxels.has_garbage(); }

        /**
         * @brief Adds a property to voxels.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value for the property.
         * @return The added property.
         */
        template<class T>
        VoxelProperty<T> add_voxel_property(const std::string &name,
                                            const T t = T()) {
            return VoxelProperty<T>(voxels.add<T>(name, t));
        }

        /**
         * @brief Retrieves a voxel property by name.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @return The property if it exists.
         */
        template<class T>
        VoxelProperty<T> get_voxel_property(const std::string &name) const {
            return VoxelProperty<T>(voxels.get<T>(name));
        }

        /**
         * @brief Retrieves or adds a vertex property.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value for the property.
         * @return The property.
         */
        template<class T>
        VoxelProperty<T> voxel_property(const std::string &name, const T t = T()) {
            return VoxelProperty<T>(voxels.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes a voxel property.
         * @tparam T Type of the property.
         * @param p Property to remove.
         */
        template<class T>
        void remove_voxelproperty(VoxelProperty<T> &p) {
            voxels.remove(p);
        }

        /**
         * @brief Checks if a voxel property exists.
         * @param name Name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_voxel_property(const std::string &name) const {
            return voxels.exists(name);
        }

        /**
         * @brief Creates a new voxel.
         *
         * This function adds a new voxel to the voxel grid by pushing back a new element
         * to the internal voxel container. It returns the newly created voxel, which is
         * represented by its index in the container.
         *
         * @return The newly created voxel.
         */
        Voxel new_voxel() {
            voxels.push_back();
            return Voxel(voxels.size() - 1);
        }

        /**
         * @brief Adds a voxel at the specified index.
         *
         * This function adds a voxel to the grid at the given index, using the provided strides
         * to compute the linear index. If a voxel already exists at the computed index, it is returned.
         * Otherwise, a new voxel is created and added to the grid.
         *
         * @tparam N The dimensionality of the voxel grid.
         * @param idx The index of the voxel in the grid.
         * @param strides The strides used to compute the linear index from the multi-dimensional index.
         * @return The voxel at the specified index.
         */
        template<int N>
        Voxel add_voxel(const Vector<int, N> &idx, const Vector<int, N> &strides) {
            size_t linear_index_key = idx.dot(strides);
            return add_voxel(linear_index_key);
        }

        /**
         * @brief Adds a voxel at the specified linear index.
         *
         * This function adds a voxel to the grid at the given linear index. If a voxel already exists
         * at the computed index, it is returned. Otherwise, a new voxel is created and added to the grid.
         *
         * @param linear_index The linear index of the voxel in the grid.
         * @return The voxel at the specified index.
         */
        Voxel add_voxel(size_t linear_index);

        /**
         * @brief Marks a voxel as deleted.
         *
         * This function sets the deletion flag for the specified voxel, indicating that it has been deleted.
         *
         * @param v Voxel to mark as deleted.
         */
        void mark_deleted(const Voxel &v);

        /**
         * @brief Deletes a voxel.
         *
         * This function removes the specified voxel from the grid and updates the internal data structures
         * to reflect the deletion.
         *
         * @param v Voxel to delete.
         * @param linear_index The linear index of the voxel to delete.
         */
        virtual void delete_voxel(const Voxel &v, size_t linear_index);
    };
}

#endif //VOXELGRID_H
