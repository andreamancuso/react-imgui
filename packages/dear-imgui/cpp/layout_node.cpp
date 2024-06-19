#include <yoga/Yoga.h>
#include <yoga/YGNode.h>
#include <yoga/YGValue.h>
#include <yoga/YGPixelGrid.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>
#include <nlohmann/json.hpp>

#include "shared.h"
#include "element.h"
#include "reactimgui.h"
#include "layout_node.h"

using json = nlohmann::json;

LayoutNode::LayoutNode(int id) : Element(id) {
    m_node = YGNodeNew();
    m_handlesChildrenWithinRenderMethod = true;
}

std::unique_ptr<LayoutNode> LayoutNode::makeNode(const json& nodeDef, ReactImgui* view) {
    auto id = nodeDef["id"].template get<int>();
    auto node = std::make_unique<LayoutNode>(id);

    if (nodeDef.contains("style") && nodeDef["style"].is_object()) {
        node->ApplyStyle(nodeDef["style"]);
    }

    return node;
};

const char* LayoutNode::GetElementType() {
    return "node";
};

void LayoutNode::InsertChild(LayoutNode* child, size_t index) {
    // printf("Yoga Linking %d (%s) to %d (%s) using index %d\n", child->m_id, child->GetElementType(), m_id, GetElementType(), index);

    YGNodeInsertChild(m_node, child->m_node, index);
};

void LayoutNode::HandleChildren(ReactImgui* view) {
    view->RenderChildren(m_id);
};

void LayoutNode::PreRender(ReactImgui* view) {};

void LayoutNode::Render(ReactImgui* view) {
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    YGNodeRef owner = YGNodeGetOwner(m_node);

    if (owner == nullptr) { // root
        YGNodeCalculateLayout(m_node, contentRegionAvail.x, contentRegionAvail.y, YGDirectionLTR);
    } else {

    }

    float left = YGNodeLayoutGetLeft(m_node);
    float top = YGNodeLayoutGetTop(m_node);
    float right = YGNodeLayoutGetRight(m_node);
    float bottom = YGNodeLayoutGetBottom(m_node);
    float width = YGNodeLayoutGetWidth(m_node);
    float height = YGNodeLayoutGetHeight(m_node);

    // YGValue leftPosition = YGNodeStyleGetPosition(m_node, YGEdgeLeft);

    YGDirection direction = YGNodeLayoutGetDirection(m_node);
    bool hadOverflow = YGNodeLayoutGetHadOverflow(m_node);

    ImGui::SetCursorPos(ImVec2(left, top)); // ?SetCursorScreenPos

    // ImGui::SetWindowPos(ImVec2(left, top));

    // ImGui::SetCursorScreenPos(ImVec2(left, top)); // ?

    ImGui::PushID(m_id);
    ImGui::BeginChild("##", ImVec2(width, height), ImGuiChildFlags_None);

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

void LayoutNode::PostRender(ReactImgui* view) {};

std::optional<YGAlign> LayoutNode::ResolveAlignItems(std::string def) {
    std::optional<YGAlign> alignItems;

    if (def == "auto") {
        alignItems = YGAlignAuto;
    } else if (def == "flex-start") {
        alignItems = YGAlignFlexStart;
    } else if (def == "center") {
        alignItems = YGAlignCenter;
    } else if (def == "flex-end") {
        alignItems = YGAlignFlexEnd;
    } else if (def == "stretch") {
        alignItems = YGAlignStretch;
    } else if (def == "baseline") {
        alignItems = YGAlignBaseline;
    }

    return alignItems;
};

size_t LayoutNode::GetChildCount() {
    return YGNodeGetChildCount(m_node);
};

void LayoutNode::ApplyStyle(const json& styleDef) {
    if (styleDef.contains("direction") && styleDef["direction"].is_string()) {
        auto rawDirection = styleDef["direction"].template get<std::string>();
        std::optional<YGDirection> direction;

        if (rawDirection == "inherit") {
            direction = YGDirectionInherit;
        } else if (rawDirection == "ltr") {
            direction = YGDirectionLTR;
        } else if (rawDirection == "rtl") {
            direction = YGDirectionRTL;
        }

        if (direction.has_value()) {
            printf("Setting %s\n", "direction");

            SetDirection(direction.value());
        }
    }
    
    if (styleDef.contains("flexDirection") && styleDef["flexDirection"].is_string()) {
        auto rawFlexDirection = styleDef["flexDirection"].template get<std::string>();
        std::optional<YGFlexDirection> flexDirection;

        if (rawFlexDirection == "column") {
            flexDirection = YGFlexDirectionColumn;
        } else if (rawFlexDirection == "column-reverse") {
            flexDirection = YGFlexDirectionColumnReverse;
        } else if (rawFlexDirection == "row") {
            flexDirection = YGFlexDirectionRow;
        } else if (rawFlexDirection == "row-reverse") {
            flexDirection = YGFlexDirectionRowReverse;
        }

        if (flexDirection.has_value()) {
            printf("Setting %s\n", "flexDirection");

            SetFlexDirection(flexDirection.value());
        }
    }
    
    if (styleDef.contains("justifyContent") && styleDef["justifyContent"].is_string()) {
        auto rawJustifyContent = styleDef["justifyContent"].template get<std::string>();
        std::optional<YGJustify> justifyContent;

        if (rawJustifyContent == "flex-start") {
            justifyContent = YGJustifyFlexStart;
        } else if (rawJustifyContent == "center") {
            justifyContent = YGJustifyCenter;
        } else if (rawJustifyContent == "flex-end") {
            justifyContent = YGJustifyFlexEnd;
        } else if (rawJustifyContent == "space-between") {
            justifyContent = YGJustifySpaceBetween;
        } else if (rawJustifyContent == "space-around") {
            justifyContent = YGJustifySpaceAround;
        } else if (rawJustifyContent == "space-evenly") {
            justifyContent = YGJustifySpaceEvenly;
        }

        if (justifyContent.has_value()) {
            printf("Setting %s: %s\n", "justifyContent", rawJustifyContent.c_str());

            SetJustifyContent(justifyContent.value());
        }
    }
    
    if (styleDef.contains("alignContent") && styleDef["alignContent"].is_string()) {
        auto rawAlignContent = styleDef["alignContent"].template get<std::string>();
        std::optional<YGAlign> alignContent;

        if (rawAlignContent == "auto") {
            alignContent = YGAlignAuto;
        } else if (rawAlignContent == "flex-start") {
            alignContent = YGAlignFlexStart;
        } else if (rawAlignContent == "center") {
            alignContent = YGAlignCenter;
        } else if (rawAlignContent == "flex-end") {
            alignContent = YGAlignFlexEnd;
        } else if (rawAlignContent == "stretch") {
            alignContent = YGAlignStretch;
        } else if (rawAlignContent == "space-between") {
            alignContent = YGAlignSpaceBetween;
        } else if (rawAlignContent == "space-around") {
            alignContent = YGAlignSpaceAround;
        } else if (rawAlignContent == "space-evenly") {
            alignContent = YGAlignSpaceEvenly;
        }

        if (alignContent.has_value()) {
            printf("Setting %s: %s\n", "alignContent", rawAlignContent.c_str());

            SetAlignContent(alignContent.value());
        }
    }
    
    if (styleDef.contains("alignItems") && styleDef["alignItems"].is_string()) {
        auto def = styleDef["alignItems"].template get<std::string>();
        std::optional<YGAlign> alignItems = ResolveAlignItems(def);

        if (alignItems.has_value()) {
            printf("Setting %s: %s\n", "alignItems", def.c_str());

            SetAlignItems(alignItems.value());
        }
    }
    
    if (styleDef.contains("alignSelf") && styleDef["alignSelf"].is_string()) {
        auto def = styleDef["alignSelf"].template get<std::string>();
        std::optional<YGAlign> alignSelf = ResolveAlignItems(def);

        if (alignSelf.has_value()) {
            printf("Setting %s: %s\n", "alignSelf", def.c_str());

            SetAlignSelf(alignSelf.value());
        }
    }
    
    if (styleDef.contains("positionType") && styleDef["positionType"].is_string()) {
        auto rawPositionType = styleDef["positionType"].template get<std::string>();
        std::optional<YGPositionType> positionType;

        if (rawPositionType == "static") {
            positionType = YGPositionTypeStatic;
        } else if (rawPositionType == "relative") {
            positionType = YGPositionTypeRelative;
        } else if (rawPositionType == "absolute") {
            positionType = YGPositionTypeAbsolute;
        }

        if (positionType.has_value()) {
            printf("Setting %s: %s\n", "positionType", rawPositionType.c_str());

            SetPositionType(positionType.value());
        }
    }
    
    if (styleDef.contains("flexWrap") && styleDef["flexWrap"].is_string()) {
        auto rawFlexWrap = styleDef["flexWrap"].template get<std::string>();
        std::optional<YGWrap> flexWrap;

        if (rawFlexWrap == "no-wrap") {
            flexWrap = YGWrapNoWrap;
        } else if (rawFlexWrap == "wrap") {
            flexWrap = YGWrapWrap;
        } else if (rawFlexWrap == "wrap-reverse") {
            flexWrap = YGWrapWrapReverse;
        }

        if (flexWrap.has_value()) {
            printf("Setting %s: %s\n", "flexWrap", rawFlexWrap.c_str());

            SetFlexWrap(flexWrap.value());
        }
    }
    
    if (styleDef.contains("overflow") && styleDef["overflow"].is_string()) {
        auto rawOverflow = styleDef["overflow"].template get<std::string>();
        std::optional<YGOverflow> overflow;

        if (rawOverflow == "visible") {
            overflow = YGOverflowVisible;
        } else if (rawOverflow == "hidden") {
            overflow = YGOverflowHidden;
        } else if (rawOverflow == "scroll") {
            overflow = YGOverflowScroll;
        }

        if (overflow.has_value()) {
            printf("Setting %s: %s\n", "overflow", rawOverflow.c_str());

            SetOverflow(overflow.value());
        }
    }
    
    if (styleDef.contains("display") && styleDef["display"].is_string()) {
        auto rawDisplay = styleDef["display"].template get<std::string>();
        std::optional<YGDisplay> display;

        if (rawDisplay == "flex") {
            display = YGDisplayFlex;
        } else if (rawDisplay == "none") {
            display = YGDisplayNone;
        }

        if (display.has_value()) {
            printf("Setting %s: %s\n", "display", rawDisplay.c_str());

            SetDisplay(display.value());
        }
    }
    
    if (styleDef.contains("flex") && styleDef["flex"].is_number()) {
        printf("Setting %s: %f\n", "flex", styleDef["flex"].template get<float>());

        SetFlex(styleDef["flex"].template get<float>());
    }
    
    if (styleDef.contains("flexGrow") && styleDef["flexGrow"].is_number()) {
        printf("Setting %s: %f\n", "flexGrow", styleDef["flexGrow"].template get<float>());

        SetFlexGrow(styleDef["flexGrow"].template get<float>());
    }
    
    if (styleDef.contains("flexShrink") && styleDef["flexShrink"].is_number()) {
        printf("Setting %s: %f\n", "flexShrink", styleDef["flexShrink"].template get<float>());

        SetFlexShrink(styleDef["flexShrink"].template get<float>());
    }
    
    if (styleDef.contains("flexBasis")) {
        if (styleDef["flexBasis"].is_number()) {
            // todo: what about percentage? Does it make sense to handle it here or below with an explicit property?
            SetFlexBasis(styleDef["flexBasis"].template get<float>());

            printf("Setting %s\n", "flexBasis");
        } else if (styleDef["flexBasis"].is_string()) {
            auto flexBasis = styleDef["display"].template get<std::string>();

            if (flexBasis == "auto") {
                printf("Setting %s\n", "auto flexBasis");

                SetFlexBasisAuto();
            }
        }
    }
    
    if (styleDef.contains("flexBasisPercent") && styleDef["flexBasisPercent"].is_number()) {
        printf("Setting %s\n", "flexBasisPercent");

        SetFlexBasisPercent(styleDef["flexBasisPercent"].template get<float>());
    }
    
    if (styleDef.contains("position") && styleDef["position"].is_object()) {
        for (auto& [key, item] : styleDef["position"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    printf("Setting %s\n", "position");

                    SetPosition(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (styleDef.contains("margin") && styleDef["margin"].is_object()) {
        for (auto& [key, item] : styleDef["margin"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    printf("Setting %s\n", "margin");

                    SetMargin(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (styleDef.contains("padding") && styleDef["padding"].is_object()) {
        for (auto& [key, item] : styleDef["padding"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    printf("Setting %s\n", "padding");

                    SetPadding(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (styleDef.contains("border") && styleDef["border"].is_object()) {
        for (auto& [key, item] : styleDef["border"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    printf("Setting %s\n", "border");

                    SetBorder(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (styleDef.contains("gap") && styleDef["gap"].is_object()) {
        for (auto& [key, item] : styleDef["gap"].items()) {
            if (item.is_number()) {
                std::optional<YGGutter> gutter = ResolveGutter(key);
                // todo: what about percentage?
                if (gutter.has_value()) {
                    printf("Setting %s\n", "gap");


                    SetGap(gutter.value(), item.template get<float>());
                }
            }
        }
    }

    if (styleDef.contains("aspectRatio") && styleDef["aspectRatio"].is_number()) {
        printf("Setting %s\n", "aspectRatio");

        SetAspectRatio(styleDef["aspectRatio"].template get<float>());
    }

    if (styleDef.contains("width")) {
        if (styleDef["width"].is_number()) {
            // todo: what about percentage?
            SetWidth(styleDef["width"].template get<float>());

            printf("Setting %s: %f\n", "width", styleDef["width"].template get<float>());
        } else if (styleDef["width"].is_string()) {
            auto width = styleDef["width"].template get<std::string>();

            if (width == "auto") {
                printf("Setting %s\n", "auto width");

                SetWidthAuto();
            } else if (width == "100%") {
                printf("Setting %s\n", "width 100pct");

                SetWidthPercent(100);
            } else if (width == "30%") {
                printf("Setting %s\n", "width 100pct");

                SetWidthPercent(30);
            }
        }
    }

    if (styleDef.contains("minWidth")) {
        if (styleDef["minWidth"].is_number()) {
            printf("Setting %s\n", "minWidth");
            // todo: what about percentage?
            SetMinWidth(styleDef["minWidth"].template get<float>());
        }
    }

    if (styleDef.contains("maxWidth")) {
        if (styleDef["maxWidth"].is_number()) {
            printf("Setting %s\n", "maxWidth");
            // todo: what about percentage?
            SetMaxWidth(styleDef["maxWidth"].template get<float>());
        }
    }

    if (styleDef.contains("height")) {
        if (styleDef["height"].is_number()) {
            printf("Setting %s: %f\n", "height", styleDef["height"].template get<float>());
            // todo: what about percentage?
            SetHeight(styleDef["height"].template get<float>());
        } else if (styleDef["height"].is_string()) {
            auto height = styleDef["height"].template get<std::string>();

            if (height == "auto") {
                printf("Setting %s\n", "auto height");
                SetHeightAuto();
            } else if (height == "100%") {
                printf("Setting %s\n", "100pct height");
                SetHeightPercent(100);
            }
        }
    }

    if (styleDef.contains("minHeight")) {
        if (styleDef["minHeight"].is_number()) {

            printf("Setting %s\n", "minHeight");
            // todo: what about percentage?
            SetMinHeight(styleDef["minHeight"].template get<float>());
        }
    }

    if (styleDef.contains("maxHeight")) {
        if (styleDef["maxHeight"].is_number()) {
            printf("Setting %s\n", "maxHeight");
            // todo: what about percentage?
            SetMaxHeight(styleDef["maxHeight"].template get<float>());
        }
    }
};

void LayoutNode::Patch(const json& nodeDef, ReactImgui* view) {
    if (nodeDef.contains("style") && nodeDef["style"].is_object()) {
        ApplyStyle(nodeDef["style"]);
    }
};

std::optional<YGEdge> LayoutNode::ResolveEdge(const std::string& edgeKey) {
    std::optional<YGEdge> edge;

    if (edgeKey == "left") {
        edge = YGEdgeLeft;
    } else if (edgeKey == "top") {
        edge = YGEdgeTop;
    } else if (edgeKey == "right") {
        edge = YGEdgeRight;
    } else if (edgeKey == "bottom") {
        edge = YGEdgeBottom;
    } else if (edgeKey == "start") {
        edge = YGEdgeStart;
    } else if (edgeKey == "end") {
        edge = YGEdgeEnd;
    } else if (edgeKey == "horizontal") {
        edge = YGEdgeHorizontal;
    } else if (edgeKey == "vertical") {
        edge = YGEdgeVertical;
    } else if (edgeKey == "all") {
        edge = YGEdgeAll;
    }

    return edge;
}

std::optional<YGGutter> LayoutNode::ResolveGutter(const std::string& gutterKey) {
    std::optional<YGGutter> gutter;

    if (gutterKey == "column") {
        gutter = YGGutterColumn;
    } else if (gutterKey == "row") {
        gutter = YGGutterRow;
    } else if (gutterKey == "all") {
        gutter = YGGutterAll;
    }

    return gutter;
};
