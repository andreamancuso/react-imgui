#include "imgui.h"
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

class MessageViewTreeView : public ImGuiView {
    json message;

    private:
        ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
        ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAllColumns;

        void RenderPayload(const json& payload)
        {
            for (auto& [key, val] : payload.items()) {
                bool is_structured = val.is_structured();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (is_structured) {
                    bool open = ImGui::TreeNodeEx(key.c_str(), tree_node_flags);
                    ImGui::TableNextColumn();
                    ImGui::TextDisabled("--");

                    if (open) {
                        RenderPayload(val);

                        ImGui::TreePop();
                    }
                } else {
                    ImGui::TreeNodeEx(key.c_str(), tree_node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
                    ImGui::TableNextColumn();

                    if (val.is_string()) {
                        ImGui::TextUnformatted(static_cast<std::string>(val).c_str());
                    } else if (val.is_number_float()) {
                        ImGui::Text("%f", static_cast<double>(val));
                    } else if (val.is_number_integer()) {
                        ImGui::Text("%d", static_cast<int>(val));
                    } else if (val.is_boolean()) {
                        if (val == true) {
                            ImGui::TextUnformatted("true");
                        } else {
                            ImGui::TextUnformatted("false");
                        }
                    } else {
                        ImGui::TextDisabled("--");
                    }
                }
            }
        }

    public:
        MessageViewTreeView(const char* newWindowId, const char* newGlWindowTitle) : ImGuiView(newWindowId, newGlWindowTitle) {

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

            // ImGui::Text("Rendering at %d frames per second", (int)io.Framerate);

            if (ImGui::BeginTable("Normalized payload", 2, flags))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool open = ImGui::TreeNodeEx("Payload", tree_node_flags);

                if (open)
                {
                    RenderPayload(message);

                    ImGui::TreePop();
                }

                ImGui::EndTable();
            }

            ImGui::End();
            ImGui::Render();
        }

        void SetData(std::string jsonData) {
            message = json::parse(jsonData);
        }

        void ResetData() {

        }
};

MessageViewTreeView view("MessageViewTreeView", "MessageViewTreeView");
GLWasm glWasm = GLWasm::GetInstance(&view);

void _exit() {
    emscripten_cancel_main_loop();
    emscripten_force_exit(0);
}

int runMain(std::string canvasSelector) {
    glWasm.Init(canvasSelector);

    return 0;
}

void sendParsedMessage(std::string jsonData)  {
    view.SetData(jsonData);
}

void resizeWindow(int width, int height) {
    glWasm.SetWindowSize(width, height);
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("runMain", &runMain);
    emscripten::function("exit", &_exit);
    emscripten::function("resizeWindow", &resizeWindow);
    emscripten::function("sendParsedMessage", &sendParsedMessage);

    emscripten::register_vector<int>("VectorInt");
    emscripten::register_vector<float>("VectorFloat");
    emscripten::register_vector<std::string>("VectorString");
    emscripten::register_map<std::string, std::vector<float>>("MapStringVector");
}
