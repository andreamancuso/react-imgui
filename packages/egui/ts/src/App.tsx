import { useMemo, useRef } from "react";
import { ReactEgui } from "./lib/components/ReactEgui/components";
import initWasmModule, { set_widget, init_egui } from "./lib/wasm/eframe_template";

import "./App.css";

const module: any = {
    initWasmModule,
    set_widget,
    init_egui,
};

function App() {
    const containerRef = useRef<HTMLDivElement>(null);

    // const arr = useMemo(() => Array.from({ length: 1000 }), []);

    return (
        <div id="app" ref={containerRef}>
            <ReactEgui initWasmModule={module} containerRef={containerRef}>
                <ReactEgui.Button label="Hello, world!" />
                <ReactEgui.InputText defaultValue="Hello, world!" />
                {/* {arr.map((_, index) => (
                    <ReactEgui.InputText key={index} defaultValue="Hello, world!" />
                ))} */}
            </ReactEgui>
        </div>
    );
}

export default App;
