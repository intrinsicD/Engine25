//
// Created by dieckman on 13.03.25.
//

#include "GuiVector.h"
#include "imgui.h"
#include "fmt/core.h"


namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const Vector<Real, 4> &v) {
        ImGui::Text(fmt::format("{}: {} {} {} {}", label, v[0], v[1], v[2], v[3]).c_str());
    }
    void ShowGui(const char *label, const Vector<Real, 3> &v) {
        ImGui::Text(fmt::format("{}: {} {} {}", label, v[0], v[1], v[2]).c_str());
    }
    void ShowGui(const char *label, const Vector<Real, 2> &v) {
        ImGui::Text(fmt::format("{}: {} {}", label, v[0], v[1]).c_str());
    }

    void EditGui(const char *label, const Vector<Real, 4> &v){
        // Select the correct ImGuiDataType and format string:
        const ImGuiDataType data_type = std::is_same<Real, float>::value ? ImGuiDataType_Float : ImGuiDataType_Double;
        const char *step_format = std::is_same<Real, float>::value ? "%.3f" : "%.6lf";
        // Edit all N components at once:
        ImGui::InputScalarN(label, data_type, (void*)v.data(), 4, step_format);
    }
    void EditGui(const char *label, const Vector<Real, 3> &v){
        // Select the correct ImGuiDataType and format string:
        const ImGuiDataType data_type = std::is_same<Real, float>::value ? ImGuiDataType_Float : ImGuiDataType_Double;
        const char *step_format = std::is_same<Real, float>::value ? "%.3f" : "%.6lf";
        // Edit all N components at once:
        ImGui::InputScalarN(label, data_type, (void*)v.data(), 3, step_format);
    }
    void EditGui(const char *label, const Vector<Real, 2> &v){
        // Select the correct ImGuiDataType and format string:
        const ImGuiDataType data_type = std::is_same<Real, float>::value ? ImGuiDataType_Float : ImGuiDataType_Double;
        const char *step_format = std::is_same<Real, float>::value ? "%.3f" : "%.6lf";
        // Edit all N components at once:
        ImGui::InputScalarN(label, data_type, (void*)v.data(), 2, step_format);
    }
}