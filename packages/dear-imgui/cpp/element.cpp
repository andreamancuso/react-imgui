// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <cstring>
#include <tuple>
#include <string>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

#include "shared.h"
#include "element.h"
#include "reactimgui.h"

using json = nlohmann::json;

Element::Element(int id) {
    m_id = id;
    m_handlesChildrenWithinRenderMethod = false;
}

void Element::Render(ReactImgui* view) {};

