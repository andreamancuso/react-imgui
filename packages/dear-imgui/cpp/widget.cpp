// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <cstring>
#include <string>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "implot.h"
#include "implot_internal.h"
#include <nlohmann/json.hpp>

#include "widget.h"
#include "reactimgui.h"

using json = nlohmann::json;

void Widget::HandleChildren(ReactImgui* view) {
    view->RenderChildren(m_id);
};

void Fragment::Render(ReactImgui* view) {
    // ImGui::Text("Begin Fragment (ID: %d)", m_id);
    Widget::HandleChildren(view);
    // ImGui::Text("End Fragment (ID: %d)", m_id);
};


void SameLine::Render(ReactImgui* view) {
    // Special case
    if (view->m_hierarchy.contains(m_id)) {
        size_t size = view->m_hierarchy[m_id].size() - 1;

        int i = 0;
        for (auto& childId : view->m_hierarchy[m_id]) {
            view->RenderWidgets(childId);

            if (i < (size)) {
                ImGui::SameLine();
            }

            i++;
        }

        // ImGui::Text("(ID: %d)", m_id);
    }
};

void Separator::Render(ReactImgui* view) {
    ImGui::Separator();
};

void Indent::Render(ReactImgui* view) {
    ImGui::Indent();
    Widget::HandleChildren(view);
    ImGui::Unindent();
};

void Unindent::Render(ReactImgui* view) {
    ImGui::Unindent();
};

void SeparatorText::Render(ReactImgui* view) {
    ImGui::SeparatorText(m_label.c_str());
};

void BulletText::Render(ReactImgui* view) {
    ImGui::BulletText(m_text.c_str());
};

void UnformattedText::Render(ReactImgui* view) {
    ImGui::TextUnformatted(m_text.c_str());
};

void DisabledText::Render(ReactImgui* view) {
    ImGui::TextDisabled(m_text.c_str());
};

void TabBar::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    // todo: double-check if we need to pass a proper id here?
    if (ImGui::BeginTabBar("", ImGuiTabBarFlags_None)) {
        Widget::HandleChildren(view);
        ImGui::EndTabBar();
    }
    ImGui::PopID();
};

void TabItem::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::BeginTabItem(m_label.c_str())) {
        Widget::HandleChildren(view);
        ImGui::EndTabItem();
    }
    ImGui::PopID();
};

void CollapsingHeader::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::CollapsingHeader(m_label.c_str())) {
        Widget::HandleChildren(view);
    }
    ImGui::PopID();
};

void TextWrap::Render(ReactImgui* view) {
    ImGui::PushTextWrapPos(m_width);
    
    Widget::HandleChildren(view);

    ImGui::PopTextWrapPos();
};

void ItemTooltip::Render(ReactImgui* view) {
    if (ImGui::BeginItemTooltip()) {
        Widget::HandleChildren(view);

        ImGui::EndTooltip();
    }
};

void TreeNode::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::TreeNode(m_label.c_str())) {
        Widget::HandleChildren(view);

        ImGui::TreePop();
        ImGui::Spacing();
    }
    ImGui::PopID();
};

void Combo::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::Combo(m_label.c_str(), &m_selectedIndex, m_itemsSeparatedByZeros.get())) {
        view->m_onComboChange(m_id, m_selectedIndex);
    }
    ImGui::PopID();
};

void InputText::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    ImGui::InputText(m_label.c_str(), m_bufferPointer.get(), 100, inputTextFlags, InputTextCb, (void*)this);
    ImGui::PopID();
};

int InputText::InputTextCb(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        auto pInputText = reinterpret_cast<InputText*>(data->UserData);

        std::string value = data->Buf;
        Widget::onInputTextChange_(pInputText->m_id, value);
    }

    return 0;
};

void Checkbox::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::Checkbox(m_label.c_str(), &m_checked)) {
        view->m_onBooleanValueChange(m_id, m_checked);
    }
    ImGui::PopID();
};

void Button::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (ImGui::Button(m_label.c_str())) {
        view->m_onClick(m_id);
    }
    ImGui::PopID();
};

void Slider::Render(ReactImgui* view) {
    ImGui::PushID(m_id);
    if (m_type == "angle") {
        if (ImGui::SliderAngle(m_label.c_str(), &m_value, m_min, m_max, "%.0f")) {
            view->m_onNumericValueChange(m_id, m_value);
        }
    } else {
        if (ImGui::SliderFloat(m_label.c_str(), &m_value, m_min, m_max, "%.0f")) {
            view->m_onNumericValueChange(m_id, m_value);
        }
    }
    ImGui::PopID();
};

void MultiSlider::Render(ReactImgui* view) {
    ImGui::PushID(m_id);

    if (m_numValues == 2) {
        if (ImGui::SliderFloat2(m_label.c_str(), m_values.get(), m_min, m_max, view->m_floatFormatChars[m_decimalDigits].get())) {
            view->m_onMultiValueChange(m_id, m_values.get(), m_numValues);
        }
    } else if (m_numValues == 3) {
        if (ImGui::SliderFloat3(m_label.c_str(), m_values.get(), m_min, m_max, view->m_floatFormatChars[m_decimalDigits].get())) {
            view->m_onMultiValueChange(m_id, m_values.get(), m_numValues);
        }
    } else if (m_numValues == 4) {
        if (ImGui::SliderFloat4(m_label.c_str(), m_values.get(), m_min, m_max, view->m_floatFormatChars[m_decimalDigits].get())) {
            view->m_onMultiValueChange(m_id, m_values.get(), m_numValues);
        }
    }

    ImGui::PopID();
};

void Table::Render(ReactImgui* view) {
    ImGui::Text("Table data length: %d", (int) m_data.size());

    ImGui::PushID(m_id);

    if (ImGui::BeginTable("t", (int)m_columns.size(), m_flags)) {
        for (const auto& columnSpec : m_columns) {
            ImGui::TableSetupColumn(columnSpec.heading.c_str(), ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
        }

        ImGui::TableHeadersRow();

        auto numColumns = m_columns.size();

        for (auto& dataRow : m_data) {
            ImGui::TableNextRow();
            for (int i = 0; i < numColumns; i++) {
                ImGui::TableSetColumnIndex(i);
                if (m_columns[i].fieldId.has_value()) {
                    auto& fieldId = m_columns[i].fieldId.value();

                    if (dataRow.contains(fieldId)) {
                        ImGui::TextUnformatted(dataRow[fieldId].c_str());
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopID();
};
