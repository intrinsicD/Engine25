//
// Created by dieckman on 13.03.25.
//

#ifndef GUIAABB_H
#define GUIAABB_H

#include "AABB.h"

namespace Bcg::Graphics::Gui {
    void ShowWidget(const AABB<Real, 3> &aabb3);
    void ShowWidget(const AABB<Real, 2> &aabb2);
}
#endif //GUIAABB_H
