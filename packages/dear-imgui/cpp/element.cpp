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

Element::Element(int id, bool isRoot) {
    m_id = id;
    m_handlesChildrenWithinRenderMethod = true;
    m_isRoot = isRoot;
    m_layoutNode = std::make_unique<LayoutNode>();
}

std::unique_ptr<Element> Element::makeElement(const json& nodeDef, ReactImgui* view) {
    auto id = nodeDef["id"].template get<int>();
    bool isRoot = (nodeDef.contains("root") && nodeDef["root"].is_boolean()) ? nodeDef["root"].template get<bool>() : false;
    auto element = std::make_unique<Element>(id, isRoot);

    if (nodeDef.is_object() && nodeDef.contains("style") && nodeDef["style"].is_object()) {
        element->m_layoutNode->ApplyStyle(nodeDef["style"]);
    }

    return element;
};

const char* Element::GetElementType() {
    return "node";
};

void Element::HandleChildren(ReactImgui* view) {
    view->RenderChildren(m_id);
};

float Element::GetLayoutLeftFromParentNode(YGNodeRef node, float left) {
    auto owner = YGNodeGetOwner(node);

    float currentNodeTop = YGNodeLayoutGetLeft(node);

    if (!YGFloatIsUndefined(currentNodeTop)) {
        left += currentNodeTop;
    }

    if (owner == nullptr) {
        return left;
    } else {
        return GetLayoutTopFromParentNode(owner, left);
    }
}

float Element::GetLayoutTopFromParentNode(YGNodeRef node, float top) {
    auto owner = YGNodeGetOwner(node);

    float currentNodeTop = YGNodeLayoutGetTop(node);

    if (!YGFloatIsUndefined(currentNodeTop)) {
        top += currentNodeTop;
    }

    if (owner == nullptr) {
        return top;
    } else {
        return GetLayoutTopFromParentNode(owner, top);
    }
}

void Element::Render(ReactImgui* view) {
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    YGNodeRef owner = YGNodeGetOwner(m_layoutNode->m_node);

    if (owner == nullptr) { // root
        YGNodeCalculateLayout(m_layoutNode->m_node, contentRegionAvail.x, contentRegionAvail.y, YGDirectionLTR);
    } else {

    }

    float left = YGNodeLayoutGetLeft(m_layoutNode->m_node);
    float top = YGNodeLayoutGetTop(m_layoutNode->m_node);
    // float right = YGNodeLayoutGetRight(m_layoutNode->m_node);
    // float bottom = YGNodeLayoutGetBottom(m_layoutNode->m_node);
    float width = YGNodeLayoutGetWidth(m_layoutNode->m_node);
    float height = YGNodeLayoutGetHeight(m_layoutNode->m_node);

    // YGValue leftPosition = YGNodeStyleGetPosition(m_node, YGEdgeLeft);

    YGDirection direction = YGNodeLayoutGetDirection(m_layoutNode->m_node);
    bool hadOverflow = YGNodeLayoutGetHadOverflow(m_layoutNode->m_node);

    ImGui::SetCursorPos(ImVec2(left, top));

    // ImGui::SetWindowPos(ImVec2(left, top));

    // ImGui::SetCursorScreenPos(ImVec2(left, top)); // ?

    ImGui::PushID(m_id);

    // ImGui::PushStyleColor(ImGuiCol_ChildBg, HEXAtoIV4("ff0000"));

    ImGui::BeginChild("##", ImVec2(width, height), ImGuiChildFlags_None);

    // ImGui::PopStyleColor();

    // ImGui::Text("%f %f", width, height);

    HandleChildren(view);

    ImGui::EndChild();
    ImGui::PopID();

    /**
float YGNodeLayoutGetMargin(YGNodeConstRef node, YGEdge edge) {
float YGNodeLayoutGetBorder(YGNodeConstRef node, YGEdge edge) {
float YGNodeLayoutGetPadding(YGNodeConstRef node, YGEdge edge) {

ImGui::BeginChild("outer_child", ImVec2(0, ImGui::GetFontSize() * 20.0f), ImGuiChildFlags_Border);
     */
};

void Element::PreRender(ReactImgui* view) {};

void Element::PostRender(ReactImgui* view) {};

void Element::Patch(const json& nodeDef, ReactImgui* view) {
    if (nodeDef.is_object() && nodeDef.contains("style") && nodeDef["style"].is_object()) {
        m_layoutNode->ApplyStyle(nodeDef["style"]);
    }
};

