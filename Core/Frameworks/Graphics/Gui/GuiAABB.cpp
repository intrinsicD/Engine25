//
// Created by dieckman on 13.03.25.
//

#include "GuiAABB.h"
#include "GuiVector.h"
#include "imgui.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const AABB<Real, 3> &aabb) {
        if(label){
            ImGui::Text("%s", label);
        }
        ShowGui("min", aabb.min());
        ShowGui("max", aabb.max());
    }

    void ShowGui(const char *label, const AABB<Real, 2> &aabb) {
        if(label){
            ImGui::Text("%s", label);
        }
        ShowGui("min", aabb.min());
        ShowGui("max", aabb.max());
    }

    void EditGui(const char *label, AABB<Real, 3> &aabb) {
        if(label){
            ImGui::Text("%s", label);
        }
        EditGui("min", aabb.min());
        EditGui("max", aabb.max());
    }

    void EditGui(const char *label, AABB<Real, 2> &aabb) {
        if(label){
            ImGui::Text("%s", label);
        }
        EditGui("min", aabb.min());
        EditGui("max", aabb.max());
    }
}