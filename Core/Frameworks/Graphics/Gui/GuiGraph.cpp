//
// Created by alex on 13.03.25.
//

#include "GuiGraph.h"
#include "GuiPropertyContainer.h"
#include "imgui.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const Graph &graph) {
        if (label) {
            ImGui::Text("%s", label);
        }
        ImGui::Text("Number of vertices: %zu", graph.vertices.n_vertices());
        ImGui::Text("Number of halfedges: %zu", graph.halfedges.n_halfedges());
        ImGui::Text("Number of edges: %zu", graph.edges.n_edges());
        if (ImGui::CollapsingHeader("Vertices")) {
            ShowGui(nullptr, graph.vertices);
        }
        if (ImGui::CollapsingHeader("Halfedges")) {
            ShowGui(nullptr, graph.halfedges);
        }
        if (ImGui::CollapsingHeader("Edges")) {
            ShowGui(nullptr, graph.edges);
        }
    }
}