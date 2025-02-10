//
// Created by alex on 2/10/25.
//

#ifndef MESHFEATURES_H
#define MESHFEATURES_H

#include "Mesh.h"

namespace Bcg {
    /**
     * @brief Detects features in the given mesh based on the specified angle.
     * @param mesh The mesh in which to detect features.
     * @param angle The angle threshold for feature detection.
     * @return The number of detected features.
     */
    size_t DetectFeatures(Mesh &mesh, Real angle);

    /**
     * @brief Detects boundary edges in the given mesh.
     * @param mesh The mesh in which to detect boundary edges.
     * @return The number of detected boundary edges.
     */
    size_t DetectBoundary(Mesh &mesh);

    /**
     * @brief Clears all detected features from the given mesh.
     * @param mesh The mesh from which to clear features.
     */
    void ClearFeatures(Mesh &mesh);
}
#endif //MESHFEATURES_H
