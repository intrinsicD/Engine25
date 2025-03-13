//
// Created by alex on 13.03.25.
//

#include "GuiSphere.h"
#include "GuiVector.h"
#include "imgui.h"

namespace Bcg::Graphics::Gui{
    void ShowGui(const char *label, const Sphere<Real, 3> &sphere){
        if(label){
            ImGui::Text("%s", label);
        }
        ShowGui("Center", sphere.center());
        ImGui::Text("Radius: %f", sphere.radius());
        ImGui::Text("Volume: %f", sphere.volume());
        ImGui::Text("Surface Area: %f", sphere.surface_area());
    }

    void ShowGui(const char *label, const Sphere<Real, 2> &circle){
        if(label){
            ImGui::Text("%s", label);
        }
        ShowGui("Center", circle.center());
        ImGui::Text("Radius: %f", circle.radius());
        ImGui::Text("Volume: %f", circle.volume());
        ImGui::Text("Surface Area: %f", circle.surface_area());
    }

    void EditGui(const char *label, Sphere<Real, 3> &sphere){
        if(label){
            ImGui::Text("%s", label);
        }
        EditGui("Center", sphere.center());
        ImGui::InputFloat("Radius", &sphere.radius());
    }

    void EditGui(const char *label, Sphere<Real, 2> &circle){
        if(label){
            ImGui::Text("%s", label);
        }
        EditGui("Center", circle.center());
        ImGui::InputFloat("Radius", &circle.radius());
    }
}