//
// Created by alex on 13.03.25.
//

#include "GuiPropertyContainer.h"
#include "imgui.h"

namespace Bcg::Graphics::Gui {
    void ShowGui(const char *label, const PropertyContainer &container) {
        if (label) {
            ImGui::Text("%s", label);
        }
        for (const auto &item: container.get_array()) {
            ImGui::Text("%s", item.first.c_str());
        }
    }
}