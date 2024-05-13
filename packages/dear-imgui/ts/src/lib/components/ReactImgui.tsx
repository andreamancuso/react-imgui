import * as React from "react";
import { useEffect, useMemo, useState, useRef, useCallback, PropsWithChildren } from "react";
import { v4 as uuidv4 } from "uuid";
import debounce from "lodash.debounce";
// @ts-ignore
import ReactNativePrivateInterface from "../react-native/ReactNativePrivateInterface";
// @ts-ignore wasm?
import getWasmModule from "../wasm/reactDearImgui.mjs";
import { MainModule, WasmExitStatus } from "../wasm/wasm-app-types";
import { ReactNativeWrapper } from "../components/ReactNativeWrapper";
import { Primitive } from "../components/ReactImgui/types";

// getWasmModule.preRun = () => {
//     ENV.MY_FILE_ROOT = "/usr/lib/test";
// };

// console.log(getWasmModule);

export type MainComponentProps = PropsWithChildren & {
    containerRef?: React.RefObject<HTMLElement>;
};

export const MainComponent: React.ComponentType<MainComponentProps> = ({
    containerRef,
    children,
}: MainComponentProps) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);

    const isWasmModuleLoading = useRef(false);
    const [wasmModule, setWasmModule] = useState<MainModule | undefined>();

    const onTextChange = useCallback(
        (id: string, value: string) => {
            const rootNodeID = id;
            const topLevelType = "onChange";
            const nativeEventParam = { value };

            ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(
                rootNodeID,
                topLevelType,
                nativeEventParam,
            );
        },
        [wasmModule],
    );

    const onComboChange = useCallback((id: string, value: number) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(
            rootNodeID,
            topLevelType,
            nativeEventParam,
        );
    }, []);

    const onNumericValueChange = useCallback((id: string, value: number) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(
            rootNodeID,
            topLevelType,
            nativeEventParam,
        );
    }, []);

    const onMultiValueChange = useCallback((id: string, values: Primitive[]) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { values };

        ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(
            rootNodeID,
            topLevelType,
            nativeEventParam,
        );
    }, []);

    const onBooleanValueChange = useCallback((id: string, value: boolean) => {
        const rootNodeID = id;
        const topLevelType = "onChange";
        const nativeEventParam = { value };

        ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(
            rootNodeID,
            topLevelType,
            nativeEventParam,
        );
    }, []);

    const onClick = useCallback((id: string) => {
        const rootNodeID = id;
        const topLevelType = "onClick";

        ReactNativePrivateInterface.nativeFabricUIManager.dispatchEvent(rootNodeID, topLevelType, {
            value: "clicked",
        });
    }, []);

    const canvasId = useMemo(() => `canvas-${uuidv4()}`, []);

    useEffect(() => {
        if (canvasRef.current && !isWasmModuleLoading.current) {
            isWasmModuleLoading.current = true;

            let localModule: MainModule;

            const load = async () => {
                const moduleArg: any = {
                    canvas: canvasRef.current, // ?
                    arguments: [`#${canvasId}`],
                    // locateFile: (path: any, scriptDirectory: any) => {
                    //     console.log(path, scriptDirectory);

                    //     return `${scriptDirectory}/lib/wasm/${path}`;
                    // },
                    eventHandlers: {
                        onTextChange,
                        onComboChange,
                        onNumericValueChange,
                        onMultiValueChange,
                        onBooleanValueChange,
                        onClick,
                    },
                };

                try {
                    localModule = await getWasmModule(moduleArg);

                    setWasmModule(localModule);
                } catch (exception) {
                    console.log("Unable to initialize the WASM correctly", exception);
                }
            };

            load();

            return () => {
                if (localModule) {
                    try {
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
        if (wasmModule) {
            if (containerRef?.current) {
                try {
                    wasmModule.resizeWindow(
                        containerRef.current.clientWidth,
                        containerRef.current.clientHeight - 62,
                    );
                } catch (exception) {
                    console.log("Unable to set initial window size");
                }
            }
        }
    }, [wasmModule]);

    useEffect(() => {
        if (wasmModule && containerRef?.current) {
            const resizeObserver = new ResizeObserver(
                debounce(() => {
                    if (containerRef.current) {
                        try {
                            wasmModule.resizeWindow(
                                containerRef.current.clientWidth,
                                containerRef.current.clientHeight - 62,
                            );
                        } catch (exception) {
                            console.log("Unable to resize window");
                        }
                    }
                }, 20),
            );

            resizeObserver.observe(containerRef.current);

            return () => resizeObserver.disconnect(); // clean up
        } else {
            return () => {};
        }
    }, [wasmModule, containerRef]);

    return (
        <>
            {wasmModule && (
                <ReactNativeWrapper wasmModule={wasmModule}>{children}</ReactNativeWrapper>
            )}
            <canvas ref={canvasRef} id={canvasId} />
        </>
    );
};
