#include <yoga/Yoga.h>
#include <yoga/YGNode.h>
#include <yoga/YGValue.h>
#include <yoga/YGPixelGrid.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>
#include <nlohmann/json.hpp>

#include "shared.h"
#include "element.h"

using json = nlohmann::json;

#pragma once

class ReactImgui;

class LayoutNode : public Element {
    public:
        YGNodeRef m_node;

        LayoutNode(int id);

        virtual const std::string& GetElementType();

        virtual void HandleChildren(ReactImgui* view);

        virtual void PreRender(ReactImgui* view);

        virtual void Render(ReactImgui* view);

        virtual void PostRender(ReactImgui* view);

        virtual void Patch(const json& elementPatchDef, ReactImgui* view);

        static std::unique_ptr<LayoutNode> makeNode(const json& val, ReactImgui* view);

        std::optional<YGAlign> ResolveAlignItems(std::string def);

        std::optional<YGEdge> ResolveEdge(const std::string& edgeKey);

        std::optional<YGGutter> ResolveGutter(const std::string& gutterKey);

        void SetDirection(YGDirection direction) {
            YGNodeStyleSetDirection(m_node, direction);
        }

        void SetFlexDirection(YGFlexDirection direction) {
            YGNodeStyleSetFlexDirection(m_node, direction);
        }

        void SetJustifyContent(YGJustify justifyContent) {
            YGNodeStyleSetJustifyContent(m_node, justifyContent);
        }

        void SetAlignContent(YGAlign alignContent) {
            YGNodeStyleSetAlignContent(m_node, alignContent);
        }

        void SetAlignItems(YGAlign alignItems) {
            YGNodeStyleSetAlignItems(m_node, alignItems);
        }

        void SetAlignSelf(YGAlign alignSelf) {
            YGNodeStyleSetAlignSelf(m_node, alignSelf);
        }

        void SetPositionType(YGPositionType positionType) {
            YGNodeStyleSetPositionType(m_node, positionType);
        }

        void SetFlexWrap(YGWrap flexWrap) {
            YGNodeStyleSetFlexWrap(m_node, flexWrap);
        }

        void SetOverflow(YGOverflow overflow) {
            YGNodeStyleSetOverflow(m_node, overflow);
        }

        void SetDisplay(YGDisplay display) {
            YGNodeStyleSetDisplay(m_node, display);
        }

        void SetFlex(float flex) {
            YGNodeStyleSetFlex(m_node, flex);
        }

        void SetFlexGrow(float flexGrow) {
            YGNodeStyleSetFlexGrow(m_node, flexGrow);
        }

        void SetFlexShrink(float flexShrink) {
            YGNodeStyleSetFlexShrink(m_node, flexShrink);
        }

        void SetFlexBasis(float flexBasis) {
            YGNodeStyleSetFlexBasis(m_node, flexBasis);
        }

        void SetFlexBasisPercent(float flexBasisPercent) {
            YGNodeStyleSetFlexBasisPercent(m_node, flexBasisPercent);
        }

        void SetFlexBasisAuto() {
            YGNodeStyleSetFlexBasisAuto(m_node);
        }

        void SetPosition(YGEdge edge, float points) {
            YGNodeStyleSetPosition(m_node, edge, points);
        }

        void SetPositionPercent(YGEdge edge, float percent) {
            YGNodeStyleSetPositionPercent(m_node, edge, percent);
        }

        void SetMargin(YGEdge edge, float points) {
            YGNodeStyleSetMargin(m_node, edge, points);
        }

        void SetMarginPercent(YGEdge edge, float percent) {
            YGNodeStyleSetMarginPercent(m_node, edge, percent);
        }

        void SetMarginAuto(YGEdge edge) {
            YGNodeStyleSetMarginAuto(m_node, edge);
        }

        void SetPadding(YGEdge edge, float points) {
            YGNodeStyleSetPadding(m_node, edge, points);
        }

        void SetPaddingPercent(YGEdge edge, float percent) {
            YGNodeStyleSetPaddingPercent(m_node, edge, percent);
        }

        void SetBorder(YGEdge edge, float border) {
            YGNodeStyleSetBorder(m_node, edge, border);
        }

        void SetGap(YGGutter gutter, float gapLength) {
            YGNodeStyleSetGap(m_node, gutter, gapLength);
        }

        void SetGapPercent(YGGutter gutter, float percent) {
            YGNodeStyleSetGapPercent(m_node, gutter, percent);
        }

        void SetAspectRatio(const float aspectRatio) {
            YGNodeStyleSetAspectRatio(m_node, aspectRatio);
        }

        void SetWidth(float width) {
            YGNodeStyleSetWidth(m_node, width);
        }

        void SetWidthPercent(float percent) {
            YGNodeStyleSetWidthPercent(m_node, percent);
        }

        void SetWidthAuto() {
            YGNodeStyleSetWidthAuto(m_node);
        }

        void SetHeight(float height) {
            YGNodeStyleSetHeight(m_node, height);
        }

        void SetHeightPercent(float percent) {
            YGNodeStyleSetHeightPercent(m_node, percent);
        }

        void SetHeightAuto() {
            YGNodeStyleSetHeightAuto(m_node);
        }

        void SetMinWidth(const float minWidth) {
            YGNodeStyleSetMinWidth(m_node, minWidth);
        }

        void SetMinWidthPercent(const float percent) {
            YGNodeStyleSetMinWidthPercent(m_node, percent);
        }

        void SetMinHeight(const float minHeight) {
            YGNodeStyleSetMinHeight(m_node, minHeight);
        }

        void SetMinHeightPercent(const float percent) {
            YGNodeStyleSetMinHeightPercent(m_node, percent);
        }

        void SetMaxWidth(const float maxWidth) {
            YGNodeStyleSetMaxWidth(m_node, maxWidth);
        }

        void SetMaxWidthPercent(const float percent) {
            YGNodeStyleSetMaxWidthPercent(m_node, percent);
        }

        void SetMaxHeight(const float maxHeight) {
            YGNodeStyleSetMaxHeight(m_node, maxHeight);
        }

        void SetMaxHeightPercent(const float percent) {
            YGNodeStyleSetMaxHeightPercent(m_node, percent);
        }
};

