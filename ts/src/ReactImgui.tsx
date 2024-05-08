import * as React from "react";
import { useEffect, useMemo, useState, useRef, useCallback, PropsWithChildren } from "react";
// @ts-ignore
import { render } from "../react-native/react-native/libraries/Renderer/implementations/ReactNativeRenderer-dev.js";
// @ts-ignore
import * as rnInterface from "../react-native/react-native/libraries/ReactPrivate/ReactNativePrivateInterface";
import { v4 as uuidv4 } from "uuid";
import debounce from "lodash.debounce";
// @ts-ignore wasm?
import getWasmModule from "./assets/reactImgui";
import { WidgetRegistrationServiceContext } from "./contexts/widgetRegistrationServiceContext";

import { MainModule, WasmExitStatus, WasmRunner } from "./wasm-app-types";
// import { render } from "./renderer/renderer";
import { WidgetRegistrationService } from "./lib/widgetRegistrationService";
import { resolveWidgets } from "./lib/resolveWidgets";
import { ImguiWidgetsFlat, JSXWidgetNodesFlat, Primitive } from "./components/ReactImgui/types";

export type MainComponentProps = PropsWithChildren & {
    containerRef?: React.RefObject<HTMLElement>;
};

export const MainComponent: React.ComponentType<MainComponentProps> = ({
    containerRef,
    children,
}: MainComponentProps) => {
    // const widgetRegistrationService = useWidgetRegistrationService();

    const widgetRegistrationServiceRef = useRef(new WidgetRegistrationService());
    // const container = useRef<any>();
    const widgetsDefsRef = useRef<any>(null);
    const canvasRef = useRef<HTMLCanvasElement>(null);

    const isWasmModuleLoading = useRef(false);
    const [module, setModule] = useState<MainModule | undefined>();
    const [wasmRunner, setWasmRunner] = useState<WasmRunner | undefined>();

    const [widgets, setWidgets] = useState<ImguiWidgetsFlat[]>([]);

    const onTextChange = useCallback((id: string, value: string) => {
        console.log(id, value);

        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType, nativeEventParam);

        // widgetRegistrationServiceRef.current.emitTextInputChangeEvent(id, value);
    }, []);

    const onComboChange = useCallback((id: string, value: number) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType, nativeEventParam);
        // setTimeout(() => {
        // widgetRegistrationServiceRef.current.emitComboChangeEvent(id, value);
        // }, 10);
    }, []);

    const onNumericValueChange = useCallback((id: string, value: number) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType, nativeEventParam);

        // setTimeout(() => {
        // widgetRegistrationServiceRef.current.emitNumericValueChangeEvent(id, value);
        // }, 10);
    }, []);

    const onMultiValueChange = useCallback((id: string, values: Primitive[]) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { values };

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType, nativeEventParam);

        // setTimeout(() => {
        // widgetRegistrationServiceRef.current.emitMultiValueChangeEvent(id, values);
        // }, 10);
    }, []);

    const onBooleanValueChange = useCallback((id: string, value: boolean) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType, nativeEventParam);
        // setTimeout(() => {
        // widgetRegistrationServiceRef.current.emitBooleanValueChangeEvent(id, value);
        // }, 10);
    }, []);

    const onClick = useCallback((id: string) => {
        const rootNodeID = id;
        const topLevelType = "onClick";

        rnInterface.RCTEventEmitter.propagateEvent(rootNodeID, topLevelType);
        // const nativeEventParam = { value };
        // setTimeout(() => {
        // widgetRegistrationServiceRef.current.emitClick(id);
        // }, 10);
    }, []);

    const setWidgetsProxy = useCallback((rawWidgetTree: JSXWidgetNodesFlat[]) => {
        setWidgets(resolveWidgets(rawWidgetTree));
    }, []);

    const canvasId = useMemo(() => `canvas-${uuidv4()}`, []);

    useEffect(() => {
        if (canvasRef.current && !isWasmModuleLoading.current) {
            isWasmModuleLoading.current = true;

            let localModule: MainModule;
            let localWasmRunner: WasmRunner;

            const load = async () => {
                const moduleArg: any = {
                    canvas: canvasRef.current,
                };

                localModule = await getWasmModule(moduleArg);

                localWasmRunner = new localModule.WasmRunner(
                    onTextChange,
                    onComboChange,
                    onNumericValueChange,
                    onMultiValueChange,
                    onBooleanValueChange,
                    onClick,
                );

                localWasmRunner.run(`#${canvasId}`);

                widgetRegistrationServiceRef.current.setFonts(
                    JSON.parse(localWasmRunner.getAvailableFonts()),
                );

                setModule(localModule);
                setWasmRunner(localWasmRunner);
            };

            load();

            return () => {
                if (localModule) {
                    try {
                        localWasmRunner.delete();
                        localModule.exit();
                    } catch (error) {
                        if ((error as WasmExitStatus).status !== 0) {
                            // TODO: report error?
                        }
                    }
                }
            };
        } else {
            return () => {};
        }
    }, [
        canvasId,
        canvasRef,
        onTextChange,
        onComboChange,
        onNumericValueChange,
        onBooleanValueChange,
        onMultiValueChange,
        onClick,
    ]);

    useEffect(() => {
        if (wasmRunner) {
            if (containerRef?.current) {
                wasmRunner.resizeWindow(
                    containerRef.current.clientWidth,
                    containerRef.current.clientHeight - 62,
                );
            }
        }
    }, [wasmRunner, widgets]);

    useEffect(() => {
        if (wasmRunner && containerRef?.current) {
            const resizeObserver = new ResizeObserver(
                debounce(() => {
                    if (containerRef.current) {
                        wasmRunner.resizeWindow(
                            containerRef.current.clientWidth,
                            containerRef.current.clientHeight - 62,
                        );
                    }
                }, 20),
            );

            resizeObserver.observe(containerRef.current);

            return () => resizeObserver.disconnect(); // clean up
        } else {
            return () => {};
        }
    }, [wasmRunner, containerRef]);

    // console.log(widgets);

    useEffect(() => {
        if (wasmRunner && !widgetsDefsRef.current) {
            rnInterface.UIManager.init(wasmRunner);

            render(
                <WidgetRegistrationServiceContext.Provider
                    value={widgetRegistrationServiceRef.current}
                >
                    {children}
                </WidgetRegistrationServiceContext.Provider>,
                0, // containerTag,
                () => {
                    console.log("initialised");
                },
            );

            // AppRegistry.registerComponent("");

            // widgetsDefsRef.current = render(
            //     <WidgetRegistrationServiceContext.Provider
            //         value={widgetRegistrationServiceRef.current}
            //     >
            //         {children}
            //     </WidgetRegistrationServiceContext.Provider>,
            //     { container: setWidgetsProxy },
            // );
        }
    }, [wasmRunner, widgetsDefsRef]);

    return (
        <>
            {/* <div id="rootEl" ref={widgetsDefsRef}></div> */}
            {/* <div>
                <pre>{JSON.stringify(widgetsDefsRef.current?.innerHTML)}</pre>
            </div> */}
            <canvas ref={canvasRef} id={canvasId} />
        </>
    );
};
