#include <cstring>
#include <string>
#include <sstream>
#include <mutex>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <rpp/rpp.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "implot.h"
#include "implot_internal.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "shared.h"
#include "element.h"
#include "implotview.h"

#pragma once

class Widget;
class LayoutNode;
struct BaseStyle;

class ReactImgui : public ImPlotView {
    private:
        std::unordered_map<int, rpp::subjects::replay_subject<TableData>> m_tableSubjects;
        std::mutex m_tableSubjectsMutex;

        std::unordered_map<std::string, std::function<std::unique_ptr<Element>(const json&, std::optional<BaseStyle>, ReactImgui*)>> m_element_init_fn;

        std::unordered_map<int, std::unique_ptr<Element>> m_elements;
        std::mutex m_elements_mutex;

        void InitElement(const json& elementDef);
        
        void SetUpFloatFormatChars();

        void SetUpElementCreatorFunctions();
        
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

        void RenderElementById(int id);

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

        void RenderElementTree(int id = 0);

        void RenderElements(int id = 0);

        void SetElement(std::string& elementJsonAsString);

        void PatchElement(int id, std::string& elementJsonAsString);

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

template <typename T, typename std::enable_if<std::is_base_of<Widget, T>::value, int>::type = 0>
std::unique_ptr<T> makeWidget(const json& val, std::optional<BaseStyle> maybeStyle, ReactImgui* view);

std::unique_ptr<Element> makeElement(const json& val, ReactImgui* view);
