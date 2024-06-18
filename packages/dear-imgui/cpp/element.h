// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <optional>
#include <tuple>
#include <cstring>
#include <string>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#pragma once

class ReactImgui;

class Element {
    public:
        int m_id;
        bool m_handlesChildrenWithinRenderMethod;

        Element(int id);

        virtual const std::string& GetElementType();

        virtual void HandleChildren(ReactImgui* view);

        virtual void PreRender(ReactImgui* view);

        virtual void Render(ReactImgui* view);

        virtual void PostRender(ReactImgui* view);

        virtual void Patch(const json& elementPatchDef, ReactImgui* view);
};

