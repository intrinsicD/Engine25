//
// Created by alex on 18.02.25.
//

#ifndef ENGINE25_CAMERA_H
#define ENGINE25_CAMERA_H

#include "GraphicsTypes.h"
#include "Math.h"

namespace Bcg::Graphics{
    struct ViewParameters{
        Vector<Real, 3> eye;
        Vector<Real, 3> center;
        Vector<Real, 3> up;
    };

    struct PerspectiveParameters{
        Real fovy_degrees;
        Real aspect;
        Real zNear;
        Real zFar;
    };

    struct OrthographicParameters{
        Real left;
        Real right;
        Real bottom;
        Real top;
        Real zNear;
        Real zFar;
    };

    struct Camera{
        Matrix<Real, 4, 4> view;
        Matrix<Real, 4, 4> proj;
    };

    struct CameraHandle{
        Uint32 id = -1;
    };

    void SetViewParameters(Camera &camera, const ViewParameters &params);

    ViewParameters GetViewParameters(const Camera &camera);

    void SetPerspectiveParameters(Camera &camera, const PerspectiveParameters &params);

    PerspectiveParameters GetPerspectiveParameters(const Camera &camera);

    void SetOrthographicParameters(Camera &camera, const OrthographicParameters &params);

    OrthographicParameters GetOrthographicParameters(const Camera &camera);

    bool IsPerspective(const CameraHandle &handle);

    bool IsOrthographic(const CameraHandle &handle);
}

#endif //ENGINE25_CAMERA_H
