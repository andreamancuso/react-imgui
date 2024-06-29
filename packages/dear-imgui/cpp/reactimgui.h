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
#include <mutex>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <rpp/rpp.hpp>
// #include <mbgl/map/map.hpp>
// #include <mbgl/map/map_options.hpp>
// #include <mbgl/gfx/headless_frontend.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "implot.h"
#include "implot_internal.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "shared.h"
#include "implotview.h"

#pragma once

class Widget;

class ReactImgui : public ImPlotView {
    private:
        std::unordered_map<int, rpp::subjects::replay_subject<TableData>> m_tableSubjects;
        std::mutex m_tableSubjectsMutex;

        std::unordered_map<std::string, std::function<std::unique_ptr<Widget>(const json&, ReactImgui*)>> m_widget_init_fn;

        std::unordered_map<int, std::unique_ptr<Widget>> m_widgets;
        std::mutex m_widgets_mutex;

        void InitWidget(const json& widgetDef);
        
        void SetUpFloatFormatChars();

        void SetUpWidgetCreatorFunctions();
        
        void HandleTableData(int id, TableData val);
        
        void HandleBufferedTableData(int id, std::vector<TableData> val);

    public:
        std::unordered_map<int, std::vector<int>> m_hierarchy;
        std::mutex m_hierarchy_mutex;

        std::unordered_map<int, std::unique_ptr<char[]>> m_floatFormatChars;

        OnTextChangedCallback m_onInputTextChange;
        OnComboChangedCallback m_onComboChange;
        OnNumericValueChangedCallback m_onNumericValueChange;
        OnMultipleNumericValuesChangedCallback m_onMultiValueChange;
        OnBooleanValueChangedCallback m_onBooleanValueChange;
        OnClickCallback m_onClick;

        ReactImgui(
            const char* newWindowId, 
            const char* newGlWindowTitle, 
            std::string& rawFontDefs,
            std::optional<std::string>& rawStyleOverridesDefs
        );

        void RenderWidgetById(int id);

        void SetEventHandlers(
            OnTextChangedCallback onInputTextChangeFn,
            OnComboChangedCallback onComboChangeFn,
            OnNumericValueChangedCallback onNumericValueChangeFn,
            OnMultipleNumericValuesChangedCallback onMultiValueChangeFn,
            OnBooleanValueChangedCallback onBooleanValueChangeFn,
            OnClickCallback onClickFn
        );

        void SetUpObservables();

        void PrepareForRender();

        void Render(int window_width, int window_height);

        void RenderChildren(int id);

        void RenderWidgets(int id = 0);

        void SetWidget(std::string& widgetJsonAsString);

        void PatchWidget(int id, std::string& widgetJsonAsString);

        void SetChildren(int id, const std::vector<int>& childIds);

        void AppendChild(int parentId, int childId);

        void AppendDataToTable(int id, std::string& data);

        void AppendTextToClippedMultiLineTextRenderer(int id, std::string& data);

        float GetTextLineHeightWithSpacing();

        std::vector<int> GetChildren(int id);

        json GetAvailableFonts();

        template <typename T>
        void ExtractNumberFromStyleDef(const json& styleDef, std::string key, T& value);

        void ExtractBooleanFromStyleDef(const json& styleDef, std::string key, bool& value);

        void ExtractImVec2FromStyleDef(const json& styleDef, std::string key, ImVec2& value);

        void PatchStyle(const json& styleDef);
};

