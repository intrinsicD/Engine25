//
// Created by dieckman on 13.03.25.
//

#include "GuiAABB.h"
#include "GuiVector.h"

namespace Bcg::Graphics::Gui {
    void ShowWidget(const AABB<Real, 3> &aabb) {
        ShowGui("min", aabb.min());
        ShowGui("max", aabb.max());
    }

    void ShowWidget(const AABB<Real, 2> &aabb) {
        ShowGui("min", aabb.min());
        ShowGui("max", aabb.max());
    }
}