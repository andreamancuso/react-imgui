import React, { useRef } from "react";
import { ReactEgui, EguiDemo } from "@react-wasm/egui";
import initWasmModule, {
    set_widget,
    init_egui,
    set_children,
    append_child,
    get_hierarchy,
    append_data_to_table,
} from "@react-wasm/egui/dist/eframe_template";

// eslint-disable-next-line import/first
import "./App.css";

// todo: improve this
const module: any = {
    initWasmModule,
    set_widget,
    set_children,
    init_egui,
    append_child,
    get_hierarchy,
    append_data_to_table,
};

function App() {
    const containerRef = useRef<HTMLDivElement>(null);

    return (
        <div id="app" ref={containerRef}>
            <ReactEgui initWasmModule={module} containerRef={containerRef}>
                <EguiDemo />
            </ReactEgui>
        </div>
    );
}

export default App;
