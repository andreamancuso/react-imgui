#include <optional>
#include <tuple>
#include <cstring>
#include <string>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

#include "layout_node.h"

using json = nlohmann::json;

#pragma once

class ReactImgui;

class Element {
    public:
        int m_id;
        bool m_handlesChildrenWithinRenderMethod;
        bool m_isRoot;
        std::unique_ptr<LayoutNode> m_layoutNode;

        Element(int id, bool isRoot);

        static std::unique_ptr<Element> makeElement(const json& val, ReactImgui* view);

        virtual const char* GetElementType();

        virtual void HandleChildren(ReactImgui* view);

        virtual void PreRender(ReactImgui* view);

        virtual void Render(ReactImgui* view);

        virtual void PostRender(ReactImgui* view);

        virtual void Patch(const json& elementPatchDef, ReactImgui* view);

        virtual float GetLayoutLeftFromParentNode(YGNodeRef node, float left = 0);

        virtual float GetLayoutTopFromParentNode(YGNodeRef node, float top = 0);
};

