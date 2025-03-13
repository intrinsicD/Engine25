//
// Created by alex on 13.03.25.
//

#include "GuiMesh.h"
#include "GuiPropertyContainer.h"
#include "imgui.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const Mesh &mesh) {
        if (label) {
            ImGui::Text("%s", label);
        }
        ImGui::Text("Number of vertices: %zu", mesh.vertices.size());
        ImGui::Text("Number of halfedges: %zu", mesh.halfedges.size());
        ImGui::Text("Number of edges: %zu", mesh.edges.size());
        ImGui::Text("Number of faces: %zu", mesh.faces.size());
        if (ImGui::CollapsingHeader("Vertices")) {
            ShowGui(nullptr, mesh.vertices);
        }
        if (ImGui::CollapsingHeader("Halfedges")) {
            ShowGui(nullptr, mesh.halfedges);
        }
        if (ImGui::CollapsingHeader("Edges")) {
            ShowGui(nullptr, mesh.edges);
        }
        if(ImGui::CollapsingHeader("Faces")){
            ShowGui(nullptr, mesh.faces);
        }
    }
}