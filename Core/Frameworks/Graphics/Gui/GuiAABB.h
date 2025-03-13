//
// Created by dieckman on 13.03.25.
//

#ifndef GUIAABB_H
#define GUIAABB_H

#include "AABB.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const AABB<Real, 3> &aabb);

    void ShowGui(const char *label, const AABB<Real, 2> &aabb);

    void EditGui(const char *label, AABB<Real, 3> &aabb);

    void EditGui(const char *label, AABB<Real, 3> &aabb);
}
#endif //GUIAABB_H
