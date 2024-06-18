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
}

std::unique_ptr<LayoutNode> LayoutNode::makeNode(const json& nodeDef, ReactImgui* view) {
    auto id = nodeDef["id"].template get<int>();

    return std::make_unique<LayoutNode>(id);
};

const std::string& LayoutNode::GetElementType() {
    return "node";
};

void LayoutNode::HandleChildren(ReactImgui* view) {};

void LayoutNode::PreRender(ReactImgui* view) {};

void LayoutNode::Render(ReactImgui* view) {};

void LayoutNode::PostRender(ReactImgui* view) {};

std::optional<YGAlign> ResolveAlignItems(const std::string& def) {
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

void LayoutNode::Patch(const json& stylePatchDef, ReactImgui* view) {
    if (stylePatchDef.contains("direction") && stylePatchDef["direction"].is_string()) {
        auto rawDirection = stylePatchDef["direction"].template get<std::string>();
        std::optional<YGDirection> direction;

        if (rawDirection == "inherit") {
            direction = YGDirectionInherit;
        } else if (rawDirection == "ltr") {
            direction = YGDirectionLTR;
        } else if (rawDirection == "rtl") {
            direction = YGDirectionRTL;
        }

        if (direction.has_value()) {
            SetDirection(direction.value());
        }
    }
    
    if (stylePatchDef.contains("flexDirection") && stylePatchDef["flexDirection"].is_string()) {
        auto rawFlexDirection = stylePatchDef["flexDirection"].template get<std::string>();
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
            SetFlexDirection(flexDirection.value());
        }
    }
    
    if (stylePatchDef.contains("justifyContent") && stylePatchDef["justifyContent"].is_string()) {
        auto rawJustifyContent = stylePatchDef["justifyContent"].template get<std::string>();
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
            SetJustifyContent(justifyContent.value());
        }
    }
    
    if (stylePatchDef.contains("alignContent") && stylePatchDef["alignContent"].is_string()) {
        auto rawAlignContent = stylePatchDef["alignContent"].template get<std::string>();
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
            SetAlignContent(alignContent.value());
        }
    }
    
    if (stylePatchDef.contains("alignItems") && stylePatchDef["alignItems"].is_string()) {
        auto def = stylePatchDef["alignItems"].template get<std::string>();
        std::optional<YGAlign> alignItems = ResolveAlignItems(def);

        if (alignItems.has_value()) {
            SetAlignItems(alignItems.value());
        }
    }
    
    if (stylePatchDef.contains("alignSelf") && stylePatchDef["alignSelf"].is_string()) {
        auto def = stylePatchDef["alignSelf"].template get<std::string>();
        std::optional<YGAlign> alignSelf = ResolveAlignItems(def);

        if (alignSelf.has_value()) {
            SetAlignSelf(alignSelf.value());
        }
    }
    
    if (stylePatchDef.contains("positionType") && stylePatchDef["positionType"].is_string()) {
        auto rawPositionType = stylePatchDef["positionType"].template get<std::string>();
        std::optional<YGPositionType> positionType;

        if (rawPositionType == "static") {
            positionType = YGPositionTypeStatic;
        } else if (rawPositionType == "relative") {
            positionType = YGPositionTypeRelative;
        } else if (rawPositionType == "absolute") {
            positionType = YGPositionTypeAbsolute;
        }

        if (positionType.has_value()) {
            SetPositionType(positionType.value());
        }
    }
    
    if (stylePatchDef.contains("flexWrap") && stylePatchDef["flexWrap"].is_string()) {
        auto rawFlexWrap = stylePatchDef["flexWrap"].template get<std::string>();
        std::optional<YGWrap> flexWrap;

        if (rawFlexWrap == "no-wrap") {
            flexWrap = YGWrapNoWrap;
        } else if (rawFlexWrap == "wrap") {
            flexWrap = YGWrapWrap;
        } else if (rawFlexWrap == "wrap-reverse") {
            flexWrap = YGWrapWrapReverse;
        }

        if (flexWrap.has_value()) {
            SetFlexWrap(flexWrap.value());
        }
    }
    
    if (stylePatchDef.contains("overflow") && stylePatchDef["overflow"].is_string()) {
        auto rawOverflow = stylePatchDef["overflow"].template get<std::string>();
        std::optional<YGOverflow> overflow;

        if (rawOverflow == "visible") {
            overflow = YGOverflowVisible;
        } else if (rawOverflow == "hidden") {
            overflow = YGOverflowHidden;
        } else if (rawOverflow == "scroll") {
            overflow = YGOverflowScroll;
        }

        if (overflow.has_value()) {
            SetOverflow(overflow.value());
        }
    }
    
    if (stylePatchDef.contains("display") && stylePatchDef["display"].is_string()) {
        auto rawDisplay = stylePatchDef["display"].template get<std::string>();
        std::optional<YGDisplay> display;

        if (rawDisplay == "flex") {
            display = YGDisplayFlex;
        } else if (rawDisplay == "none") {
            display = YGDisplayNone;
        }

        if (display.has_value()) {
            SetDisplay(display.value());
        }
    }
    
    if (stylePatchDef.contains("flex") && stylePatchDef["flex"].is_number()) {
        SetFlex(stylePatchDef["flex"].template get<float>());
    }
    
    if (stylePatchDef.contains("flexGrow") && stylePatchDef["flexGrow"].is_number()) {
        SetFlexGrow(stylePatchDef["flexGrow"].template get<float>());
    }
    
    if (stylePatchDef.contains("flexShrink") && stylePatchDef["flexShrink"].is_number()) {
        SetFlexShrink(stylePatchDef["flexShrink"].template get<float>());
    }
    
    if (stylePatchDef.contains("flexBasis")) {
        if (stylePatchDef["flexBasis"].is_number()) {
            // todo: what about percentage? Does it make sense to handle it here or below with an explicit property?
            SetFlexBasis(stylePatchDef["flexBasis"].template get<float>());
        } else if (stylePatchDef["flexBasis"].is_string()) {
            auto flexBasis = stylePatchDef["display"].template get<std::string>();

            if (flexBasis == "auto") {
                SetFlexBasisAuto();
            }
        }
    }
    
    if (stylePatchDef.contains("flexBasisPercent") && stylePatchDef["flexBasisPercent"].is_number()) {
        SetFlexBasisPercent(stylePatchDef["flexBasisPercent"].template get<float>());
    }
    
    if (stylePatchDef.contains("position") && stylePatchDef["position"].is_object()) {
        for (auto& [key, item] : stylePatchDef["position"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    SetPosition(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (stylePatchDef.contains("margin") && stylePatchDef["margin"].is_object()) {
        for (auto& [key, item] : stylePatchDef["margin"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    SetMargin(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (stylePatchDef.contains("padding") && stylePatchDef["padding"].is_object()) {
        for (auto& [key, item] : stylePatchDef["padding"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    SetPadding(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (stylePatchDef.contains("border") && stylePatchDef["border"].is_object()) {
        for (auto& [key, item] : stylePatchDef["border"].items()) {
            if (item.is_number()) {
                std::optional<YGEdge> edge = ResolveEdge(key);
                // todo: what about percentage?
                if (edge.has_value()) {
                    SetBorder(edge.value(), item.template get<float>());
                }
            }
        }
    }
    
    if (stylePatchDef.contains("gap") && stylePatchDef["gap"].is_object()) {
        for (auto& [key, item] : stylePatchDef["gap"].items()) {
            if (item.is_number()) {
                std::optional<YGGutter> gutter = ResolveGutter(key);
                // todo: what about percentage?
                if (gutter.has_value()) {
                    SetGap(gutter.value(), item.template get<float>());
                }
            }
        }
    }

    if (stylePatchDef.contains("aspectRatio") && stylePatchDef["aspectRatio"].is_number()) {
        SetAspectRatio(stylePatchDef["aspectRatio"].template get<float>());
    }

    if (stylePatchDef.contains("width")) {
        if (stylePatchDef["width"].is_number()) {
            // todo: what about percentage?
            SetWidth(stylePatchDef["width"].template get<float>());
        } else if (stylePatchDef["width"].is_string()) {
            auto width = stylePatchDef["width"].template get<std::string>();

            if (width == "auto") {
                SetWidthAuto();
            }
        }
    }

    if (stylePatchDef.contains("minWidth")) {
        if (stylePatchDef["minWidth"].is_number()) {
            // todo: what about percentage?
            SetMinWidth(stylePatchDef["minWidth"].template get<float>());
        }
    }

    if (stylePatchDef.contains("maxWidth")) {
        if (stylePatchDef["maxWidth"].is_number()) {
            // todo: what about percentage?
            SetMaxWidth(stylePatchDef["maxWidth"].template get<float>());
        }
    }

    if (stylePatchDef.contains("height")) {
        if (stylePatchDef["height"].is_number()) {
            // todo: what about percentage?
            SetHeight(stylePatchDef["height"].template get<float>());
        } else if (stylePatchDef["height"].is_string()) {
            auto height = stylePatchDef["height"].template get<std::string>();

            if (height == "auto") {
                SetHeightAuto();
            }
        }
    }

    if (stylePatchDef.contains("minHeight")) {
        if (stylePatchDef["minHeight"].is_number()) {
            // todo: what about percentage?
            SetMinHeight(stylePatchDef["minHeight"].template get<float>());
        }
    }

    if (stylePatchDef.contains("maxHeight")) {
        if (stylePatchDef["maxHeight"].is_number()) {
            // todo: what about percentage?
            SetMaxHeight(stylePatchDef["maxHeight"].template get<float>());
        }
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
