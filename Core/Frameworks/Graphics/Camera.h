//
// Created by alex on 18.02.25.
//

#ifndef ENGINE25_CAMERA_H
#define ENGINE25_CAMERA_H

#include "GraphicsTypes.h"
#include "Math.h"

namespace Bcg::Graphics {
    struct ViewParameters {
        Vector<Real, 3> eye;
        Vector<Real, 3> center;
        Vector<Real, 3> up;
    };

    struct PerspectiveParameters {
        Real fovy_degrees;
        Real aspect;
        Real zNear;
        Real zFar;
    };

    struct OrthographicParameters {
        Real left;
        Real right;
        Real bottom;
        Real top;
        Real zNear;
        Real zFar;
    };

    struct Camera {
        Matrix<Real, 4, 4> view;
        Matrix<Real, 4, 4> proj;
    };

    struct CameraHandle {
        Uint32 id = -1;
    };

    void SetViewParameters(Camera &camera, const ViewParameters &params);

    ViewParameters GetViewParameters(const Camera &camera);

    void SetPerspectiveParameters(Camera &camera, const PerspectiveParameters &params);

    PerspectiveParameters GetPerspectiveParameters(const Camera &camera);

    void SetOrthographicParameters(Camera &camera, const OrthographicParameters &params);

    OrthographicParameters GetOrthographicParameters(const Camera &camera);

    OrthographicParameters ApproxConvertFrom(const PerspectiveParameters &params, float depth_z) {
        float half_height = depth_z * tanf(ToRadians(params.fovy_degrees) / 2.0f);
        float half_width = half_height * params.aspect;

        OrthographicParameters result;
        result.left = -half_width;
        result.right = half_width;
        result.bottom = -half_height;
        result.top = half_height;
        result.zNear = params.zNear;
        result.zFar = params.zFar;
        return result;
    }

    PerspectiveParameters ApproxConvertFrom(const OrthographicParameters &params, float depth_z) {
        float fov = 2.0f * atanf(params.top / depth_z);
        float aspect = (params.right - params.left) / (params.top - params.bottom);

        PerspectiveParameters result;
        result.fovy_degrees = ToDegrees(fov);
        result.aspect = aspect;
        result.zNear = params.zNear;
        result.zFar = params.zFar;
        return result;
    }

    bool IsPerspective(const CameraHandle &handle);

    bool IsOrthographic(const CameraHandle &handle);
}

#endif //ENGINE25_CAMERA_H
