#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

#include <nlohmann/json.hpp>

#include "../common/glwasm.cpp"
#include "../common/imguiview.cpp"

using json = nlohmann::json;

struct ColumnSpec {
    std::string identifier;
};

// emscripten::val onColumnClear;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ColumnSpec, identifier);
// const char* str
EM_JS(void, onColumnClear, (const char* identifier), {
    Module['onColumnClear'](UTF8ToString(identifier));
});

// void clearColumn(std::string columnField) {
//     // TODO: THIS BREAKS WHEN ASSERTIONS===1
//     // Specifically, dispatch(setViewSettings(...))
//     // onColumnClear(emscripten::val(columnField));
//     onColumnClear(columnField);
// }

class TableView : public ImGuiView {
    private:
        ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

        std::vector<ColumnSpec> columnSpecs;

    public:
        TableView(const char* newWindowId, const char* newGlWindowTitle) : ImGuiView(newWindowId, newGlWindowTitle) {

        }

        void PrepareForRender() {
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
            io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("fonts/Cousine-Regular.ttf", 15.0f);
            //io.Fonts->AddFontFromFileTTF("fonts/ProggyTiny.ttf", 10.0f);
            //ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
            //IM_ASSERT(font != nullptr);
        #endif

            ImGui::StyleColorsLight();
        }

        void Render(int window_width, int window_height) {
            SetCurrentContext();

            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(window_width, window_height));

            ImGui::Begin(windowId, NULL, window_flags);

            if (columnSpecs.size()) {
                if (ImGui::BeginTable("t", (int)columnSpecs.size(), flags)) {

                        for (const ColumnSpec& columnSpec : columnSpecs) {
                            ImGui::TableSetupColumn(columnSpec.identifier.c_str(), ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
                        }

                        ImGui::TableHeadersRow();


                    // ImGui::TableNextRow();
                    // ImGui::TableNextColumn();

                    ImGuiTable* table = imGuiCtx->CurrentTable;
                    table->DisableDefaultContextMenu = true;
                    if (ImGui::TableBeginContextMenuPopup(table)) {
                        const int column_n = (table->ContextPopupColumn >= 0 && table->ContextPopupColumn < table->ColumnsCount) ? table->ContextPopupColumn : -1;
                        ImGuiTableColumn* column = (column_n != -1) ? &table->Columns[column_n] : NULL;

                        if (column != NULL) {
                            if (ImGui::MenuItem("Remove column", NULL)) {
                                RemoveColumn(column_n);
                            }
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::EndTable();
                }
            } else {
                ImGui::TextUnformatted("No columns.");
            }

            ImGui::End();
            ImGui::Render();
        }

        void SetColumnSpecs(std::vector<ColumnSpec> newColumnSpecs) {
            // columnSpecs = newColumnSpecs;

            columnSpecs.clear();

            for (const ColumnSpec& columnSpec : newColumnSpecs) {
                columnSpecs.push_back(columnSpec);
            }
        }

        void RemoveColumn(int columnIndex) {
            onColumnClear(columnSpecs.at(columnIndex).identifier.c_str());
        }

        void AppendData() {

        }

        void ResetData() {

        }
};

TableView view("TableView", "TableView");
GLWasm glWasm = GLWasm::GetInstance(&view);

int runMain(std::string canvasSelector) {
    glWasm.Init(canvasSelector);

    return 0;
}

void _exit() {
    emscripten_cancel_main_loop();
    emscripten_force_exit(0);
}

void resetData() {
    view.ResetData();
}

void appendData()  {
    view.AppendData();
}

void setColumnSpecs(std::string jsonData) {
    json columnSpecsJson = json::parse(jsonData);
    auto columnSpecs = columnSpecsJson.template get<std::vector<ColumnSpec>>();

    view.SetColumnSpecs(columnSpecs);
}

void resizeWindow(int width, int height) {
    glWasm.SetWindowSize(width, height);
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("runMain", &runMain);
    emscripten::function("exit", &_exit);
    emscripten::function("resizeWindow", &resizeWindow);
    emscripten::function("resetData", &resetData);
    emscripten::function("appendData", &appendData);
    emscripten::function("setColumnSpecs", &setColumnSpecs);

    emscripten::register_vector<int>("VectorInt");
    emscripten::register_vector<float>("VectorFloat");
    emscripten::register_vector<std::string>("VectorString");
    emscripten::register_map<std::string, std::vector<float>>("MapStringVector");

    // emscripten::register_type<OnColumnClear>("(column: string) => void");
}
