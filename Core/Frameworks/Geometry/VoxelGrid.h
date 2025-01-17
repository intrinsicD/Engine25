//
// Created by alex on 1/17/25.
//

#ifndef VOXELGRID_H
#define VOXELGRID_H

#include "GeometricProperties.h"

namespace Bcg {
    class VoxelGrid {
    public:
        class VoxelLinearIndex : public Handle {
        public:
            using Handle::Handle;
        };

        std::unordered_map<VoxelLinearIndex, Voxel> sparse_voxels_map;
        VertexProperty<bool> v_deleted; ///< Deletion flag for voxels.
        Voxels voxels;

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
        [[nodiscard]] size_t n_voxels() const { return voxels.size(); }

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
         * @return The newly created voxel.
         */
        Voxel new_voxel() {
            voxels.push_back();
            return Voxel(voxels.size() - 1);
        }

        /**
         * @brief Adds a voxel with a specified position.
         * @param p Position of the new voxel.
         * @return The newly created voxel.
         */
        Voxel add_voxel(const Vector<Real, 3> &p);

        /**
        * @brief Marks a voxel as deleted.
        * @param v Vertex to mark as deleted.
        */
        void mark_deleted(const Voxel &v);

        /**
         * @brief Deletes a voxel.
         * @param v Voxel to delete.
         */
        virtual void delete_voxel(const Voxel &v);
    };
}

#endif //VOXELGRID_H
