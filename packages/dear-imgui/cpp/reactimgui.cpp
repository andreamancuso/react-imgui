// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <cstring>
#include <string>
#include <functional>
#include <concepts>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <rpp/rpp.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "implot.h"
#include "implot_internal.h"
#include <nlohmann/json.hpp>

#include "shared.h"
#include "reactimgui.h"
#include "implotview.h"
#include "widget.h"
#include "layout_node.h"

using json = nlohmann::json;

template <typename T, typename std::enable_if<std::is_base_of<Widget, T>::value, int>::type>
std::unique_ptr<T> makeWidget(const json& val, ReactImgui* view) {
    return T::makeWidget(val, view);
}

std::unique_ptr<LayoutNode> makeNode(const json& val, ReactImgui* view) {
    return LayoutNode::makeNode(val, view);
}

ReactImgui::ReactImgui(
    const char* newWindowId, 
    const char* newGlWindowTitle, 
    std::string& rawFontDefs,
    std::optional<std::string>& rawStyleOverridesDefs
) : ImPlotView(newWindowId, newGlWindowTitle, rawFontDefs) {
    SetUpElementCreatorFunctions();
    SetUpFloatFormatChars();
    SetUpObservables();

    if (rawStyleOverridesDefs.has_value()) {
        m_shouldLoadDefaultStyle = false;
        PatchStyle(json::parse(rawStyleOverridesDefs.value()));
    }
}

void ReactImgui::SetUpObservables() {
    // Do we need this?
};

void ReactImgui::SetUpElementCreatorFunctions() {
    m_element_init_fn["Node"] = &makeNode;

    m_element_init_fn["Group"] = &makeWidget<Group>;
    m_element_init_fn["Child"] = &makeWidget<Child>;
    m_element_init_fn["DIWindow"] = &makeWidget<Window>;
    m_element_init_fn["Fragment"] = &makeWidget<Fragment>;
    m_element_init_fn["Indent"] = &makeWidget<Indent>;
    m_element_init_fn["SameLine"] = &makeWidget<SameLine>;
    m_element_init_fn["Separator"] = &makeWidget<Separator>;

    m_element_init_fn["CollapsingHeader"] = &makeWidget<CollapsingHeader>;
    m_element_init_fn["TabBar"] = &makeWidget<TabBar>;
    m_element_init_fn["TabItem"] = &makeWidget<TabItem>;
    m_element_init_fn["TreeNode"] = &makeWidget<TreeNode>;

    m_element_init_fn["Table"] = &makeWidget<Table>;
    m_element_init_fn["ClippedMultiLineTextRenderer"] = &makeWidget<ClippedMultiLineTextRenderer>;

    m_element_init_fn["ItemTooltip"] = &makeWidget<ItemTooltip>;

    m_element_init_fn["Combo"] = &makeWidget<Combo>;
    m_element_init_fn["Slider"] = &makeWidget<Slider>;
    m_element_init_fn["InputText"] = &makeWidget<InputText>;
    m_element_init_fn["MultiSlider"] = &makeWidget<MultiSlider>;
    m_element_init_fn["Checkbox"] = &makeWidget<Checkbox>;
    m_element_init_fn["Button"] = &makeWidget<Button>;
    
    m_element_init_fn["SeparatorText"] = &makeWidget<SeparatorText>;
    m_element_init_fn["BulletText"] = &makeWidget<BulletText>;
    m_element_init_fn["UnformattedText"] = &makeWidget<UnformattedText>;
    m_element_init_fn["DisabledText"] = &makeWidget<DisabledText>;
    m_element_init_fn["TextWrap"] = &makeWidget<TextWrap>;
};

void ReactImgui::RenderElementById(int id) {
    m_elements[id]->PreRender(this);
    m_elements[id]->Render(this);
    m_elements[id]->PostRender(this);
};

void ReactImgui::RenderElements(int id) {
    if (m_elements.contains(id)) {
        RenderElementById(id);
    }

    if (!m_elements.contains(id) || m_elements[id]->m_handlesChildrenWithinRenderMethod == false) {
        RenderChildren(id);
    }
};

void ReactImgui::RenderChildren(int id) {
    if (m_hierarchy.contains(id)) {
        if (m_hierarchy[id].size() > 0) {
            for (auto& childId : m_hierarchy[id]) {
                RenderElements(childId);
            }
        }
    }
};

void ReactImgui::InitElement(const json& widgetDef) {
    if (widgetDef.is_object() && widgetDef.contains("type")) {
        std::string type = widgetDef["type"].template get<std::string>();

        if (m_element_init_fn.contains(type)) {
            int id = widgetDef["id"].template get<int>();

            const std::lock_guard<std::mutex> widgetLock(m_elements_mutex);
            const std::lock_guard<std::mutex> hierarchyLock(m_hierarchy_mutex);

            m_elements[id] = m_element_init_fn[type](widgetDef, this);
            m_hierarchy[id] = std::vector<int>();

            if (type == "Table") {
                const std::lock_guard<std::mutex> lock(m_tableSubjectsMutex);

                m_tableSubjects[id] = rpp::subjects::replay_subject<TableData>{100};

                // auto handler = std::bind(&ReactImgui::HandleBufferedTableData, this, id, std::placeholders::_1);
                auto handler = std::bind(&ReactImgui::HandleTableData, this, id, std::placeholders::_1);

                // todo: restore buffer() usage
                // m_tableSubjects[id].get_observable() | rpp::ops::buffer(50) | rpp::ops::subscribe(handler);
                m_tableSubjects[id].get_observable() | rpp::ops::subscribe(handler);
            }
        } else {
            printf("unrecognised widget type: '%s'\n", type.c_str());
        }
    } else {
        printf("received JSON either not an object or does not contain type property\n");
    }
};

void ReactImgui::HandleTableData(int id, TableData val) {
    // printf("%d\n", (int)val.size());

    static_cast<Table*>(m_elements[id].get())->AppendData(val);
};

void ReactImgui::HandleBufferedTableData(int id, std::vector<TableData> val) {
    // printf("%d\n", (int)val.size()); // I'm seeing 50 the first time this gets called, then 1 subsequent times...

    const std::lock_guard<std::mutex> widgetLock(m_elements_mutex);

    size_t totalSize = 0;

    for (const auto& chunk : val) {
        totalSize += chunk.size();
    }

    TableData data;

    data.reserve(totalSize);

    for (const auto& chunk : val) {
        data.insert(data.end(), chunk.begin(), chunk.end());
    }

    static_cast<Table*>(m_elements[id].get())->AppendData(data);
};

void ReactImgui::SetEventHandlers(
    OnTextChangedCallback onInputTextChangeFn,
    OnComboChangedCallback onComboChangeFn,
    OnNumericValueChangedCallback onNumericValueChangeFn,
    OnMultipleNumericValuesChangedCallback onMultiValueChangeFn,
    OnBooleanValueChangedCallback onBooleanValueChangeFn,
    OnClickCallback onClickFn
) {
    m_onInputTextChange = onInputTextChangeFn;
    m_onComboChange = onComboChangeFn;
    m_onNumericValueChange = onNumericValueChangeFn;
    m_onMultiValueChange = onMultiValueChangeFn;
    m_onBooleanValueChange = onBooleanValueChangeFn;
    m_onClick = onClickFn;

    Widget::onInputTextChange_ = onInputTextChangeFn;
};

void ReactImgui::SetUpFloatFormatChars() {
    m_floatFormatChars[0] = std::make_unique<char[]>(4);
    m_floatFormatChars[1] = std::make_unique<char[]>(4);
    m_floatFormatChars[2] = std::make_unique<char[]>(4);
    m_floatFormatChars[3] = std::make_unique<char[]>(4);
    m_floatFormatChars[4] = std::make_unique<char[]>(4);
    m_floatFormatChars[5] = std::make_unique<char[]>(4);
    m_floatFormatChars[6] = std::make_unique<char[]>(4);
    m_floatFormatChars[7] = std::make_unique<char[]>(4);
    m_floatFormatChars[8] = std::make_unique<char[]>(4);
    m_floatFormatChars[9] = std::make_unique<char[]>(4);

    strcpy(m_floatFormatChars[0].get(), "%.0f");
    strcpy(m_floatFormatChars[1].get(), "%.1f");
    strcpy(m_floatFormatChars[2].get(), "%.2f");
    strcpy(m_floatFormatChars[3].get(), "%.3f");
    strcpy(m_floatFormatChars[4].get(), "%.4f");
    strcpy(m_floatFormatChars[5].get(), "%.5f");
    strcpy(m_floatFormatChars[6].get(), "%.6f");
    strcpy(m_floatFormatChars[7].get(), "%.7f");
    strcpy(m_floatFormatChars[8].get(), "%.8f");
    strcpy(m_floatFormatChars[9].get(), "%.9f");
};

void ReactImgui::PrepareForRender() {
    SetCurrentContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
#endif

#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
    //io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
#endif

    if (m_shouldLoadDefaultStyle) {
        ImGui::StyleColorsLight();
        // ImGui::StyleColorsDark();
    }
};

void ReactImgui::Render(int window_width, int window_height) {
    SetCurrentContext();

    // ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    const std::lock_guard<std::mutex> widgetsLock(m_elements_mutex);
    const std::lock_guard<std::mutex> hierarchyLock(m_hierarchy_mutex);

    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));

    ImGui::Begin(m_windowId, NULL, m_window_flags);

    RenderElements();

    ImGui::End();
    ImGui::Render();
};

template <typename T>
void ReactImgui::ExtractNumberFromStyleDef(const json& styleDef, std::string key, T& value) {
    if (styleDef.contains(key) && styleDef[key].is_number_unsigned()) {
        value = styleDef[key].template get<T>();
    }
};

void ReactImgui::ExtractBooleanFromStyleDef(const json& styleDef, std::string key, bool& value) {
    if (styleDef.contains(key) && styleDef[key].is_number_unsigned()) {
        value = styleDef[key].template get<bool>();
    }
};

void ReactImgui::ExtractImVec2FromStyleDef(const json& styleDef, std::string key, ImVec2& value) {
    if (styleDef.contains(key)  && styleDef[key].is_array() && styleDef[key].size() == 2) {
        value.x = styleDef[key][0].template get<float>();
        value.y = styleDef[key][1].template get<float>();
    }
};

void ReactImgui::PatchStyle(const json& styleDef) {
    if (styleDef.is_object()) {
        ImGuiStyle* style = &GetStyle();

        ExtractNumberFromStyleDef<float>(styleDef, "alpha", style->Alpha);
        ExtractNumberFromStyleDef<float>(styleDef, "disabledAlpha", style->DisabledAlpha);
        ExtractImVec2FromStyleDef(styleDef, "windowPadding", style->WindowPadding);
        ExtractNumberFromStyleDef<float>(styleDef, "windowRounding", style->WindowRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "windowBorderSize", style->WindowBorderSize);
        ExtractImVec2FromStyleDef(styleDef, "windowMinSize", style->WindowMinSize);
        ExtractImVec2FromStyleDef(styleDef, "windowTitleAlign", style->WindowTitleAlign);
        ExtractNumberFromStyleDef<int>(styleDef, "windowMenuButtonPosition", style->WindowMenuButtonPosition);
        ExtractNumberFromStyleDef<float>(styleDef, "childRounding", style->ChildRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "childBorderSize", style->ChildBorderSize);
        ExtractNumberFromStyleDef<float>(styleDef, "popupRounding", style->PopupRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "popupBorderSize", style->PopupBorderSize);
        ExtractImVec2FromStyleDef(styleDef, "framePadding", style->FramePadding);
        ExtractNumberFromStyleDef<float>(styleDef, "frameRounding", style->FrameRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "frameBorderSize", style->FrameBorderSize);
        ExtractImVec2FromStyleDef(styleDef, "itemSpacing", style->ItemSpacing);
        ExtractImVec2FromStyleDef(styleDef, "itemInnerSpacing", style->ItemInnerSpacing);
        ExtractImVec2FromStyleDef(styleDef, "cellPadding", style->CellPadding);
        ExtractImVec2FromStyleDef(styleDef, "touchExtraPadding", style->TouchExtraPadding);
        ExtractNumberFromStyleDef<float>(styleDef, "indentSpacing", style->IndentSpacing);
        ExtractNumberFromStyleDef<float>(styleDef, "columnsMinSpacing", style->ColumnsMinSpacing);
        ExtractNumberFromStyleDef<float>(styleDef, "scrollbarSize", style->ScrollbarSize);
        ExtractNumberFromStyleDef<float>(styleDef, "scrollbarRounding", style->ScrollbarRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "grabMinSize", style->GrabMinSize);
        ExtractNumberFromStyleDef<float>(styleDef, "grabRounding", style->GrabRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "logSliderDeadzone", style->LogSliderDeadzone);
        ExtractNumberFromStyleDef<float>(styleDef, "tabRounding", style->TabRounding);
        ExtractNumberFromStyleDef<float>(styleDef, "tabBorderSize", style->TabBorderSize);
        ExtractNumberFromStyleDef<float>(styleDef, "tabMinWidthForCloseButton", style->TabMinWidthForCloseButton);
        ExtractNumberFromStyleDef<float>(styleDef, "tabBarBorderSize", style->TabBarBorderSize);
        ExtractNumberFromStyleDef<float>(styleDef, "tableAngledHeadersAngle", style->TableAngledHeadersAngle);
        ExtractImVec2FromStyleDef(styleDef, "tableAngledHeadersTextAlign", style->TableAngledHeadersTextAlign);
        ExtractNumberFromStyleDef<int>(styleDef, "colorButtonPosition", style->ColorButtonPosition);
        ExtractImVec2FromStyleDef(styleDef, "buttonTextAlign", style->ButtonTextAlign);
        ExtractImVec2FromStyleDef(styleDef, "selectableTextAlign", style->SelectableTextAlign);
        ExtractNumberFromStyleDef<float>(styleDef, "separatorTextBorderSize", style->SeparatorTextBorderSize);
        ExtractImVec2FromStyleDef(styleDef, "separatorTextAlign", style->SeparatorTextAlign);
        ExtractImVec2FromStyleDef(styleDef, "separatorTextPadding", style->SeparatorTextPadding);
        ExtractImVec2FromStyleDef(styleDef, "displayWindowPadding", style->DisplayWindowPadding);
        ExtractImVec2FromStyleDef(styleDef, "displaySafeAreaPadding", style->DisplaySafeAreaPadding);
        ExtractNumberFromStyleDef<float>(styleDef, "mouseCursorScale", style->MouseCursorScale);
        ExtractBooleanFromStyleDef(styleDef, "antiAliasedLines", style->AntiAliasedLines);
        ExtractBooleanFromStyleDef(styleDef, "antiAliasedLinesUseTex", style->AntiAliasedLinesUseTex);
        ExtractBooleanFromStyleDef(styleDef, "antiAliasedFill", style->AntiAliasedFill);
        ExtractNumberFromStyleDef<float>(styleDef, "curveTessellationTol", style->CurveTessellationTol);
        ExtractNumberFromStyleDef<float>(styleDef, "circleTessellationMaxError", style->CircleTessellationMaxError);
        ExtractNumberFromStyleDef<float>(styleDef, "hoverStationaryDelay", style->HoverStationaryDelay);
        ExtractNumberFromStyleDef<float>(styleDef, "hoverDelayShort", style->HoverDelayShort);
        ExtractNumberFromStyleDef<float>(styleDef, "hoverDelayNormal", style->HoverDelayNormal);
        ExtractNumberFromStyleDef<int>(styleDef, "hoverFlagsForTooltipMouse", style->HoverFlagsForTooltipMouse);
        ExtractNumberFromStyleDef<int>(styleDef, "hoverFlagsForTooltipNav", style->HoverFlagsForTooltipNav);

        if (styleDef.contains("colors") && styleDef["colors"].is_object()) {
            ImVec4* colors = style->Colors;

            for (auto& [colorItemKey, colorItemValue] : styleDef["colors"].items()) {
                auto colorItemKeyAsNumber = stoi(colorItemKey);

                if (colorItemKeyAsNumber >= 0 && colorItemKeyAsNumber < ImGuiCol_COUNT 
                    && colorItemValue.is_array() && colorItemValue.size() == 2) {
                    colors[colorItemKeyAsNumber] = HEXAtoIV4(
                        colorItemValue[0].template get<std::string>().c_str(),
                        colorItemValue[1].template get<float>()
                    );
                }
            }
        }
    }
};

void ReactImgui::SetElement(std::string& widgetJsonAsString) {
    InitElement(json::parse(widgetJsonAsString));
};

void ReactImgui::PatchElement(int id, std::string& widgetJsonAsString) {
    const std::lock_guard<std::mutex> lock(m_elements_mutex);

    if (m_elements.contains(id)) {
        auto widgetDef = json::parse(widgetJsonAsString);
        auto pWidget = m_elements[id].get();

        pWidget->Patch(widgetDef, this);
    }
};

void ReactImgui::SetChildren(int id, const std::vector<int>& childrenIds) {
    const std::lock_guard<std::mutex> lock(m_hierarchy_mutex);
    m_hierarchy[id] = childrenIds;
};

void ReactImgui::AppendChild(int parentId, int childId) {
    if (m_hierarchy.contains(parentId)) {
        if ( std::find(m_hierarchy[parentId].begin(), m_hierarchy[parentId].end(), childId) == m_hierarchy[parentId].end() ) {
            const std::lock_guard<std::mutex> lock(m_hierarchy_mutex);
            m_hierarchy[parentId].push_back(childId);
        }
    }
};

std::vector<int> ReactImgui::GetChildren(int id) {
    return m_hierarchy[id];
};

json ReactImgui::GetAvailableFonts() {
    SetCurrentContext();
    ImGuiIO& io = ImGui::GetIO();
    json fonts = json::array();

    for (ImFont* font : io.Fonts->Fonts) {
        fonts.push_back(font->GetDebugName());

        printf("%s\n", font->GetDebugName());
    }

    return fonts;
};

void ReactImgui::AppendDataToTable(int id, std::string& rawData) {
    if (m_tableSubjects.contains(id)) {
        const std::lock_guard<std::mutex> lock(m_tableSubjectsMutex);
        auto parsedData = json::parse(rawData);

        if (parsedData.is_array()) {
            TableData data = TableData();

            for (auto& [parsedItemKey, parsedRow] : parsedData.items()) {
                if (parsedRow.is_object()) {
                    TableRow row = TableRow();

                    for (auto& [parsedRowFieldKey, parsedRowFieldValue] : parsedRow.items()) {
                        if (parsedRowFieldValue.is_string()) {
                            row[parsedRowFieldKey] = parsedRowFieldValue.template get<std::string>();
                        }
                    }

                    data.push_back(row);
                }
            }

            // printf("About to add data to subject\n");
            m_tableSubjects[id].get_observer().on_next(data);
            // printf("Added data to subject\n");
        }
    } else {
        // todo: should we lock beforehand?
        // todo: should we throw here, or return a boolean to indicate whether the append operation was successfully 'queued' success or failure
    }
};

// todo: switch to ReactivePlusPlus's BehaviorSubject
void ReactImgui::AppendTextToClippedMultiLineTextRenderer(int id, std::string& rawData) {
    const std::lock_guard<std::mutex> lock(m_elements_mutex);

    if (m_elements.contains(id) && m_elements[id]->GetElementType() == "widget") {
        auto element = static_cast<Widget*>(m_elements[id].get());

        if (element->m_type == "ClippedMultiLineTextRenderer") {
            static_cast<ClippedMultiLineTextRenderer*>(m_elements[id].get())->AppendText(rawData.c_str());
        }
    }
};

float ReactImgui::GetTextLineHeightWithSpacing() {
    return ImGui::GetTextLineHeightWithSpacing();
};
