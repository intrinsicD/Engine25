//
// Created by alex on 1/18/25.
//

#include "VoxelGrid.h"

namespace Bcg {
    VoxelGrid::VoxelGrid() : voxels(), sparse_voxels_map() {
        v_deleted = voxel_property<bool>("v:deleted", false);
        v_linear_index = voxel_property<size_t>("v:linear_index", 0);
    }


    VoxelGrid &VoxelGrid::operator=(const VoxelGrid &rhs) {
        if (this != &rhs) {
            voxels = rhs.voxels;
            sparse_voxels_map = rhs.sparse_voxels_map;
            v_deleted = rhs.v_deleted;
            v_linear_index = rhs.v_linear_index;
        }
        return *this;
    }

    VoxelGrid &VoxelGrid::assign(const VoxelGrid &rhs) {
        if (this != &rhs) {
            // clear properties
            clear();

            // resize (needed by property containers)
            voxels.resize(rhs.voxels.size());
            sparse_voxels_map = rhs.sparse_voxels_map;

            v_deleted.vector() = rhs.v_deleted.vector();
            v_linear_index.vector() = rhs.v_linear_index.vector();

            // how many elements are deleted?
            voxels.num_deleted = rhs.voxels.num_deleted;
        }

        return *this;
    }

    void VoxelGrid::clear() {
        voxels.clear();
        sparse_voxels_map.clear();
        free_memory();

        v_deleted = voxel_property<bool>("v:deleted", false);
        v_linear_index = voxel_property<size_t>("v:linear_index", 0);
    }

    void VoxelGrid::free_memory() {
        voxels.free_memory();
    }

    void VoxelGrid::reserve(size_t nvoxels) {
        voxels.reserve(nvoxels);
        sparse_voxels_map.reserve(nvoxels);
    }

    void VoxelGrid::garbage_collection() {
        if (!voxels.has_garbage())
            return;

        auto nV = voxels.size();

        // remove deleted voxels
        if (nV > 0) {
            size_t i0 = 0;
            size_t i1 = nV - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!v_deleted[Voxel(i0)] && i0 < i1)
                    ++i0;
                while (v_deleted[Voxel(i1)] && i0 < i1)
                    --i1;
                if (i0 >= i1)
                    break;

                // swap
                voxels.swap(i0, i1);
            }


            // remember new size
            nV = v_deleted[Voxel(i0)] ? i0 : i0 + 1;

            voxels.resize(nV);
            free_memory();

            voxels.num_deleted = 0;

            //update sparse_voxels_map
            sparse_voxels_map.clear();
            for (size_t i = 0; i < voxels.size(); ++i) {
                sparse_voxels_map[v_linear_index[Voxel(i)]] = Voxel(i);
            }
        }
    }

    // -------------------------------------------------------------------------------------------------------------
    // Voxel Methods
    // -------------------------------------------------------------------------------------------------------------

    void VoxelGrid::mark_deleted(const Voxel &v) {
        if (v_deleted[v])
            return;

        v_deleted[v] = true;
        ++voxels.num_deleted;
    }

    void VoxelGrid::delete_voxel(const Voxel &v, size_t linear_index) {
        mark_deleted(v);
        sparse_voxels_map.erase(linear_index);
    }
}
