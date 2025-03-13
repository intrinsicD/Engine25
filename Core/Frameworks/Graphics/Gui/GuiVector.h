//
// Created by dieckman on 13.03.25.
//

#ifndef GUIVECTOR_H
#define GUIVECTOR_H

#include "Math.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const Vector<Real, 4> &v);
    void ShowGui(const char *label, const Vector<Real, 3> &v);
    void ShowGui(const char *label, const Vector<Real, 2> &v);

    void EditGui(const char *label, const Vector<Real, 4> &v);
    void EditGui(const char *label, const Vector<Real, 3> &v);
    void EditGui(const char *label, const Vector<Real, 2> &v);
}

#endif //GUIVECTOR_H
