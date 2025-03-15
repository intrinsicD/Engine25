//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_POINTCLOUD_H
#define ENGINE25_POINTCLOUD_H

#include "Vertices.h"

namespace Bcg {
    /**
     * @class PointCloud
     * @brief A class for managing 3D point clouds, handling vertices and associated properties.
     */
    class PointCloud : public Vertices{
    public:
        VertexProperty<Vector<Real, 3> > positions; ///< Position property for each vertex.
        VertexProperty<bool> v_deleted; ///< Deletion flag for vertices.

        /**
         * @brief Default constructor. Initializes properties and containers.
         */
        PointCloud();

        /**
         * @brief Virtual destructor for extensibility.
         */
        ~PointCloud() override = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another PointCloud instance to copy.
         */
        PointCloud(const PointCloud &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another PointCloud instance to copy.
         * @return Reference to this PointCloud.
         */
        PointCloud &operator=(const PointCloud &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another PointCloud instance to copy.
         * @return Reference to this PointCloud.
         */
        PointCloud &assign(const PointCloud &rhs);

        /**
         * @brief Clears all vertices and properties.
         */
        void clear() override;

        /**
         * @brief Adds a vertex with a specified position.
         * @param p Position of the new vertex.
         * @return The newly created vertex.
         */
        Vertex add_vertex(const Vector<Real, 3> &p);
    };
}

#endif //ENGINE25_POINTCLOUD_H
