export default class {
    wasmModule;
    dispatchEventFn;
    cloningNode;
    fiberNodesMap;

    constructor() {
        this.fiberNodesMap = new Map();
    }

    init(wasmModule, widgetRegistrationService) {
        this.wasmModule = wasmModule;
        this.widgetRegistrationService = widgetRegistrationService;
    }
    dispatchEvent = (rootNodeID, topLevelType, nativeEventParam) => {
        setTimeout(() => {
            this.dispatchEventFn(
                this.fiberNodesMap.get(rootNodeID),
                topLevelType,
                nativeEventParam,
            );
        }, 0);
    };
    createNode = (generatedId, uiViewClassName, requiresClone, payload, fiberNode) => {
        // todo: yikes
        if (this.cloningNode) {
            this.cloningNode = null;
        }

        const { children, type, id, ...props } = payload;

        if (type === "Table") {
            console.log(
                "createNode",
                generatedId,
                uiViewClassName,
                requiresClone,
                payload,
                fiberNode,
            );
        }

        const widget = { ...props, id: generatedId, type };

        this.wasmModule.setWidget(JSON.stringify(widget));

        this.fiberNodesMap.set(generatedId, fiberNode);

        // todo: type is in some array of types
        if (type === "Table") {
            this.widgetRegistrationService.linkWidgetIds(id, generatedId);
        }

        return widget;
    };
    cloneNodeWithNewProps = (node, newProps) => {
        const newWidget = { ...node, ...newProps };
        this.wasmModule.patchWidget(node.id, JSON.stringify(newWidget));

        return newWidget;
    };
    // This is a rather problematic method
    // I should drop all children then re-append them but naturally this causes out of memory bounds issues
    // Also, I still don't understand at which point I am supposed to delete widgets/nodes
    // todo: have a look at react-native's own implementation of this...
    cloneNodeWithNewChildren = (node) => {
        // todo: yikes
        if (this.cloningNode) {
            this.wasmModule.setChildren(
                this.cloningNode.id,
                JSON.stringify(this.cloningNode.childrenIds),
            );
        }

        this.cloningNode = { id: node.id, childrenIds: [] };

        // todo: does this make sense?
        return node;
    };
    createChildSet(...args) {
        // console.log("createChildSet", args);

        return [];
    }
    appendChildToSet(set, child) {
        // console.log("appendChildToSet", set, child);

        set.push(child);
    }
    appendChild = (parent, child) => {
        // console.log("appendChild", parent, child);

        // todo: yikes
        if (this.cloningNode) {
            this.cloningNode.childrenIds.push(child.id);
        } else {
            this.wasmModule.appendChild(parent.id, child.id);
        }
    };
    completeRoot = (container, newChildSet) => {
        // console.log("completeRoot", container, newChildSet);

        // todo: yikes
        if (this.cloningNode) {
            const cloningNodeId = this.cloningNode.id;
            const payload = JSON.stringify(this.cloningNode.childrenIds);
            this.wasmModule.setChildren(cloningNodeId, payload);
            this.cloningNode = null;
        }

        const payload = JSON.stringify(newChildSet.map(({ id }) => id));

        this.wasmModule.setChildren(container, payload);
    };
    registerEventHandler = (dispatchEventFn) => {
        this.dispatchEventFn = dispatchEventFn;
    };
}
