import React, { useCallback, useMemo, useRef, useState } from "react";
import { HelpMarker } from "./HelpMarker/HelpMarker";
import { ReactImgui } from "src/lib/components/ReactImgui/components";
import { UserGuide } from "./UserGuide/UserGuide";
import { StyleEditor } from "./StyleEditor/StyleEditor";
import { TableImperativeHandle } from "../ReactImgui/Table";

export const ImGuiDemo = () => {
    const tableRef = useRef<TableImperativeHandle>(null);
    const [text, setText] = useState("Hello, world!");
    const [tripleSliderValue, setTripleSliderValue] = useState<[number, number, number]>([9, 9, 9]);
    const [quadSliderValue, setQuadSliderValue] = useState<[number, number, number, number]>([
        9, 9, 9, 9,
    ]);

    const handleInputTextChanged = useCallback((event: any) => {
        if (event?.nativeEvent) {
            setText(String(event?.nativeEvent.value));
        }
    }, []);

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

    const tableColumns = useMemo(
        () => [
            {
                heading: "ID",
                fieldId: "id",
            },
            {
                heading: "Name",
                fieldId: "name",
            },
        ],
        [],
    );

    const handleAppendDataToTableClick = useCallback(() => {
        if (tableRef.current) {
            tableRef.current.appendDataToTable([{ id: "1", name: "Andy" }]);
        }
    }, [tableRef]);

    return (
        <ReactImgui.Fragment>
            <ReactImgui.UnformattedText text="dear imgui says hello!" />

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

            <ReactImgui.DIWindow title="another window">
                <ReactImgui.SameLine>
                    <ReactImgui.Child width={400} height={0}>
                        <ReactImgui.InputText
                            defaultValue={text}
                            onChange={handleInputTextChanged}
                        />
                        <ReactImgui.UnformattedText text={text} />
                    </ReactImgui.Child>
                    <ReactImgui.Child width={400} height={0}>
                        <ReactImgui.Table ref={tableRef} columns={tableColumns} />
                        <ReactImgui.Button
                            onClick={handleAppendDataToTableClick}
                            label="Add data to table"
                        />
                    </ReactImgui.Child>
                </ReactImgui.SameLine>
            </ReactImgui.DIWindow>
        </ReactImgui.Fragment>
    );
};
