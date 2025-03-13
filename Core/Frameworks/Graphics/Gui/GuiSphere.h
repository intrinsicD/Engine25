//
// Created by alex on 13.03.25.
//

#ifndef ENGINE25_GUISPHERE_H
#define ENGINE25_GUISPHERE_H

#include "Sphere.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const Sphere<Real, 3> &sphere);

    void ShowGui(const char *label, const Sphere<Real, 2> &circle);

    void EditGui(const char *label, Sphere<Real, 3> &sphere);

    void EditGui(const char *label, Sphere<Real, 2> &circle);
}

#endif //ENGINE25_GUISPHERE_H
