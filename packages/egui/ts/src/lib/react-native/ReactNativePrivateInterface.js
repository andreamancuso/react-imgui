import NativeFabricUIManager from "./nativeFabricUiManager.js";
import deepDiffer from "./deepDiffer.js";

const uiManager = new NativeFabricUIManager();

export default {
    createPublicInstance(current, renderLanes, workInProgress) {
        // console.log("createPublicInstance", current, renderLanes, workInProgress);

        return {};
    },
    get BatchedBridge() {
        return {};
    },
    get ExceptionsManager() {
        return {};
    },
    get Platform() {
        return {};
    },
    get ReactNativeViewConfigRegistry() {
        return {
            customBubblingEventTypes: {},
            customDirectEventTypes: {
                onChange: { registrationName: "onChange" },
                onClick: { registrationName: "onClick" },
            },
            get(elementType, ...unknownArgs) {
                switch (elementType) {
                    case "widget":
                        return {
                            // todo: perhaps it's time to define all widgets as separate entities
                            validAttributes: {
                                type: true,
                                id: true,
                                label: true,
                                tooltipText: true,
                                text: true,
                                defaultValues: true,
                                defaultValue: true,
                                min: true,
                                max: true,
                                width: true,
                                options: true,
                                sliderType: true,
                                numValues: true,
                                decimalDigits: true,
                                defaultChecked: true,
                                size: true,
                                onChange: true,
                                onClick: true,
                                columns: true,
                            },
                        };
                }

                return {
                    validAttributes: [],
                };
            },
        };
    },
    get TextInputState() {
        return {};
    },
    get nativeFabricUIManager() {
        return uiManager;
    },
    // TODO: Remove when React has migrated to `createAttributePayload` and `diffAttributePayloads`
    get deepDiffer() {
        return deepDiffer;
    },
    get deepFreezeAndThrowOnMutationInDev() {
        // Applicable only in DEV mode
        // return (...args) => console.log("deepFreezeAndThrowOnMutationInDev", args);
        return (...args) => {};
    },
    get flattenStyle() {
        return {};
    },
    get ReactFiberErrorDialog() {
        return {
            showErrorDialog(...args) {
                console.log("ReactFiberErrorDialog.showErrorDialog", args);
            },
        };
    },
    get legacySendAccessibilityEvent() {
        return {};
    },
    get RawEventEmitter() {
        return {
            emit(...args) {
                // console.log(args);
            },
        };
    },
    get CustomEvent() {
        return {};
    },
    get createAttributePayload() {
        return {};
    },
    get diffAttributePayloads() {
        return {};
    },
    get createPublicTextInstance() {
        return {};
    },
    get getNativeTagFromPublicInstance() {
        return {};
    },
    get getNodeFromPublicInstance() {
        return {};
    },
    get getInternalInstanceHandleFromPublicInstance() {
        return {};
    },
};
