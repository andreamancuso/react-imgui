import { ReplaySubject, Subscription } from "rxjs";
import { MainModule } from "../wasm/wasm-app-types";
import { WidgetRegistrationService } from "../widgetRegistrationService";

type CloningNode = { id: number; childrenIds: number[] } | null;
type DispatchEventFn = (id: number, topLevelType: string, nativeEventParam: any) => void;
type Event = [number, string, any];

export default class {
    wasmModule?: MainModule;
    dispatchEventFn?: DispatchEventFn;
    cloningNode?: CloningNode;
    fiberNodesMap: Map<number, any>;
    widgetRegistrationService?: WidgetRegistrationService;
    eventSubject: ReplaySubject<Event>;
    eventSubjectSubscription: Subscription;

    linkedWidgetTypes = ["Table", "ClippedMultiLineTextRenderer"];

    constructor() {
        this.fiberNodesMap = new Map();
        this.eventSubject = new ReplaySubject<Event>(10);

        this.eventSubjectSubscription = this.eventSubject.subscribe(
            ([rootNodeID, topLevelType, nativeEventParam]) => {
                if (this.dispatchEventFn) {
                    this.dispatchEventFn(
                        this.fiberNodesMap.get(rootNodeID),
                        topLevelType,
                        nativeEventParam,
                    );
                }
            },
        );
    }

    destroy() {
        this.eventSubjectSubscription.unsubscribe();
    }

    init(wasmModule: MainModule, widgetRegistrationService: WidgetRegistrationService) {
        this.wasmModule = wasmModule;
        this.widgetRegistrationService = widgetRegistrationService;
    }
    dispatchEvent = (rootNodeID: number, topLevelType: string, nativeEventParam: any) => {
        this.eventSubject.next([rootNodeID, topLevelType, nativeEventParam]);
    };
    createNode = (
        generatedId: number,
        uiViewClassName: string,
        requiresClone: boolean,
        payload: any,
        fiberNode: any,
    ) => {
        // todo: yikes
        if (this.cloningNode) {
            this.cloningNode = null;
        }

        const { children, type, id, elementType, ...props } = payload;

        // currently elementType can be either 'node' or 'widget'

        console.log("createNode", generatedId, uiViewClassName, requiresClone, payload, fiberNode);

        const widget = {
            ...props,
            id: generatedId,
            type: fiberNode.type === "node" ? "Node" : type,
        };

        console.log(JSON.stringify(widget));

        this.wasmModule?.setWidget(JSON.stringify(widget));

        this.fiberNodesMap.set(generatedId, fiberNode);

        // todo: type is in some array of types
        if (this.linkedWidgetTypes.includes(type)) {
            this.widgetRegistrationService?.linkWidgetIds(id, generatedId);
        }

        return widget;
    };
    cloneNodeWithNewProps = (node: any, newProps: any) => {
        const newWidget = { ...node, ...newProps };
        this.wasmModule?.patchWidget(node.id, JSON.stringify(newWidget));

        return newWidget;
    };
    // This is a rather problematic method
    // I should drop all children then re-append them but naturally this causes out of memory bounds issues
    // Also, I still don't understand at which point I am supposed to delete widgets/nodes
    // todo: have a look at react-native's own implementation of this...
    cloneNodeWithNewChildren = (node: any) => {
        // todo: yikes
        if (this.cloningNode) {
            this.wasmModule?.setChildren(
                this.cloningNode.id,
                JSON.stringify(this.cloningNode.childrenIds),
            );
        }

        this.cloningNode = { id: node.id, childrenIds: [] };

        // todo: does this make sense?
        return node;
    };
    createChildSet(...args: any[]) {
        // console.log("createChildSet", args);

        return [];
    }
    appendChildToSet(set: any[], child: any) {
        // console.log("appendChildToSet", set, child);

        set.push(child);
    }
    appendChild = (parent: any, child: any) => {
        // console.log("appendChild", parent, child);

        // todo: yikes
        if (this.cloningNode) {
            this.cloningNode.childrenIds.push(child.id);
        } else {
            this.wasmModule?.appendChild(parent.id, child.id);
        }
    };
    completeRoot = (container: any, newChildSet: any) => {
        // console.log("completeRoot", container, newChildSet);

        // todo: yikes
        if (this.cloningNode) {
            const cloningNodeId = this.cloningNode.id;
            const payload = JSON.stringify(this.cloningNode.childrenIds);
            this.wasmModule?.setChildren(cloningNodeId, payload);
            this.cloningNode = null;
        }

        const payload = JSON.stringify(newChildSet.map(({ id }: { id: number }) => id));

        this.wasmModule?.setChildren(container, payload);
    };
    registerEventHandler = (dispatchEventFn: DispatchEventFn) => {
        this.dispatchEventFn = dispatchEventFn;
    };
}
