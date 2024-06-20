import React, { SyntheticEvent, useCallback, useMemo, useRef, useState } from "react";
import { HelpMarker } from "./HelpMarker/HelpMarker";
import { ReactImgui } from "src/lib/components/ReactImgui/components";
import { UserGuide } from "./UserGuide/UserGuide";
import { StyleEditor } from "./StyleEditor/StyleEditor";
import { TableImperativeHandle } from "../ReactImgui/Table";
import { ClippedMultiLineTextRendererImperativeHandle } from "../ReactImgui/ClippedMultiLineTextRenderer";
import faIconMap from "../../fa-icons";
import RWStyleSheet from "../../stylesheet/stylesheet";
import { Tables } from "./Tables/Tables";
import { ImGuiCol, ImGuiStyleVar } from "src/lib/wasm/wasm-app-types";
import { WidgetReactElement } from "../ReactImgui/types";

export const ImGuiDemo = () => {
    const [color, setColor] = useState("ff6e59");
    const styleSheet = useMemo(
        () =>
            RWStyleSheet.create({
                title: {
                    colors: { [ImGuiCol.Text]: "ff6e59" },
                    font: { name: "roboto-regular", size: 24 },
                },
                clippedText: {
                    colors: { [ImGuiCol.Text]: color },
                },
                inputWindow: {
                    vars: { [ImGuiStyleVar.WindowPadding]: [20, 10] },
                },
            }),
        [color],
    );

    const handleToggleColorClicked = useCallback(
        () => setColor((currentColor) => (currentColor === "ff6e59" ? "1a1a1a" : "ff6e59")),
        [],
    );

    const clippedMultiLineTextRendererRef =
        useRef<ClippedMultiLineTextRendererImperativeHandle>(null);
    const [text, setText] = useState("Hello, world!");
    const [tripleSliderValue, setTripleSliderValue] = useState<[number, number, number]>([0, 0, 0]);
    const [quadSliderValue, setQuadSliderValue] = useState<[number, number, number, number]>([
        0, 0, 0, 0,
    ]);

    const handleInputTextChanged = useCallback(
        (event: SyntheticEvent<WidgetReactElement<"InputText">, { value: string }>) => {
            if (event?.nativeEvent) {
                setText(String(event?.nativeEvent.value));
            }
        },
        [],
    );

    const handleTripleSliderValueChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setTripleSliderValue([
                event.nativeEvent.values[0],
                event.nativeEvent.values[1],
                event.nativeEvent.values[2],
            ]);
        }
    }, []);

    const handleQuadSliderValueChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setQuadSliderValue([
                event.nativeEvent.values[0],
                event.nativeEvent.values[1],
                event.nativeEvent.values[2],
                event.nativeEvent.values[3],
            ]);
        }
    }, []);

    const handleAppendTextToTextRenderer = useCallback(() => {
        if (clippedMultiLineTextRendererRef.current) {
            clippedMultiLineTextRendererRef.current.appendTextToClippedMultiLineTextRenderer(
                `Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n`.repeat(
                    100,
                ),
            );
        }
    }, [clippedMultiLineTextRendererRef]);

    return (
        <ReactImgui.Fragment>
            {/* <ReactImgui.UnformattedText
                text={`dear imgui says hello! ${faIconMap["address-book"]} ${faIconMap["wine-bottle"]}`}
                style={styleSheet.title}
            />

            <ReactImgui.CollapsingHeader label="Help">
                <ReactImgui.SeparatorText label="ABOUT THIS DEMO:" />
                <ReactImgui.BulletText text="Sections below are demonstrating many aspects of the library." />
                <ReactImgui.BulletText text='The "Examples" menu above leads to more demo contents.' />
                <ReactImgui.BulletText
                    text={`The "Tools" menu above gives access to: About Box, Style Editor,\nand Metrics/Debugger (general purpose Dear ImGui debugging tool).`}
                />
                <ReactImgui.SeparatorText label="PROGRAMMER GUIDE:" />
                <ReactImgui.BulletText text="See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!" />
                <ReactImgui.BulletText text="See comments in imgui.cpp." />
                <ReactImgui.BulletText text="See example applications in the examples/ folder." />
                <ReactImgui.BulletText text="Read the FAQ at https://www.dearimgui.com/faq/" />
                <ReactImgui.BulletText text="Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls." />
                <ReactImgui.BulletText text="Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls." />

                <ReactImgui.SeparatorText label="USER GUIDE:" />
                <UserGuide />
            </ReactImgui.CollapsingHeader>
            <ReactImgui.CollapsingHeader label="Configuration">
                <ReactImgui.TreeNode label="Configuration">
                    <ReactImgui.UnformattedText text="Coming soon!" />
                </ReactImgui.TreeNode>
                <ReactImgui.TreeNode label="Backend Flags">
                    <HelpMarker
                        text={`Those flags are set by the backends (imgui_impl_xxx files) to specify their capabilities.\nHere we expose them as read-only fields to avoid breaking interactions with your backend.`}
                    />
                    <ReactImgui.UnformattedText text="Coming soon!" />
                </ReactImgui.TreeNode>
                <ReactImgui.TreeNode label="Style">
                    <HelpMarker text="The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function." />
                    <StyleEditor />
                </ReactImgui.TreeNode>
            </ReactImgui.CollapsingHeader>
            <ReactImgui.CollapsingHeader label="Window options">
                <ReactImgui.UnformattedText text="Coming soon!" />
            </ReactImgui.CollapsingHeader>
            <ReactImgui.CollapsingHeader label="Widgets">
                <ReactImgui.SameLine>
                    <ReactImgui.UnformattedText text="Triple Slider" />
                    <ReactImgui.MultiSlider
                        numValues={3}
                        onChange={handleTripleSliderValueChanged}
                    />
                    <ReactImgui.UnformattedText text={tripleSliderValue.join(", ")} />
                </ReactImgui.SameLine>
                <ReactImgui.SameLine>
                    <ReactImgui.UnformattedText text="Quadruple Slider" />
                    <ReactImgui.MultiSlider numValues={4} onChange={handleQuadSliderValueChanged} />
                    <ReactImgui.UnformattedText text={quadSliderValue.join(", ")} />
                </ReactImgui.SameLine>
            </ReactImgui.CollapsingHeader>

            <ReactImgui.DIWindow title="clipped multi line text renderer" width={820} height={600}>
                <ReactImgui.Child height={-40}>
                    <ReactImgui.ClippedMultiLineTextRenderer
                        ref={clippedMultiLineTextRendererRef}
                        style={styleSheet.clippedText}
                    />
                </ReactImgui.Child>
                <ReactImgui.SameLine>
                    <ReactImgui.Button onClick={handleAppendTextToTextRenderer} label="Add text" />
                    <ReactImgui.Button onClick={handleToggleColorClicked} label="Toggle color" />
                </ReactImgui.SameLine>
            </ReactImgui.DIWindow>

            <ReactImgui.DIWindow
                title="input widgets"
                width={820}
                height={600}
                style={styleSheet.inputWindow}
            >
                <ReactImgui.Child width={400} height={0}>
                    <ReactImgui.InputText
                        defaultValue={text}
                        onChange={handleInputTextChanged}
                        style={{ width: 0.8 }}
                    />
                    <ReactImgui.UnformattedText text={text} />
                </ReactImgui.Child>
            </ReactImgui.DIWindow> */}

            <ReactImgui.DIWindow title="tables" width={1000} height={600}>
                <Tables />
            </ReactImgui.DIWindow>
        </ReactImgui.Fragment>
    );
};
