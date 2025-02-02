//
// Created by alex on 20.01.25.
//

#ifndef ENGINE25_VOXELGRIDDOWNSAMPLING_H
#define ENGINE25_VOXELGRIDDOWNSAMPLING_H

#include "VoxelGrid.h"

namespace Bcg {
/**
 * @brief Class for downsampling a set of 3D positions using a voxel grid.
 */
class VoxelGridDownsampling {
public:
    /**
     * @brief Constructs a VoxelGridDownsampling object with the given positions.
     * @param positions The input positions to be downsampled.
     */
    explicit VoxelGridDownsampling(const std::vector<Vector<Real, 3> > &positions);

    /**
     * @brief Builds the voxel grid based on the given axis-aligned bounding box and voxel sizes.
     * @param aabb The axis-aligned bounding box defining the grid boundaries.
     * @param voxel_sizes The sizes of the voxels in each dimension.
     * @return True if the grid was successfully built, false otherwise.
     */
    bool build_grid(const AABB<Real, 3> &aabb, const Vector<Real, 3> &voxel_sizes);

    /**
     * @brief Returns the downsampled positions after voxel grid processing.
     * @return A vector of downsampled positions.
     */
    std::vector<Vector<Real, 3> > downsampled_positions();

private:
    const std::vector<Vector<Real, 3> > positions; ///< The input positions to be downsampled.
    VoxelProperty<Vector<Real, 3> > integrated_positions; ///< Integrated positions within each voxel.
    VoxelProperty<int> counts; ///< Count of points within each voxel.
    VoxelGrid grid; ///< The voxel grid used for downsampling.
};
}

#endif //ENGINE25_VOXELGRIDDOWNSAMPLING_H
