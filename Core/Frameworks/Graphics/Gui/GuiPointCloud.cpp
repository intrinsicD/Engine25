//
// Created by alex on 13.03.25.
//

#include "GuiPointCloud.h"
#include "imgui.h"
#include "GuiPropertyContainer.h"

namespace Bcg::Graphics::Gui{
    void ShowGui(const char *label, const PointCloud &pc) {
        if(label){
            ImGui::Text("%s", label);
        }
        ImGui::Text("Number of points: %zu", pc.vertices.n_vertices());
        auto positions = pc.get_vertex_property<Vector<Real, 3>>("v:position");
        if(positions){
            ImGui::Text("Number of dimensions: %zu", positions.dims());
        }
        if(ImGui::CollapsingHeader("Vertices")){
            ShowGui(nullptr, pc.vertices);
        }
    }
}