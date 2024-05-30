import { useRef } from "react";
import { useWidgetRegistrationService } from "../../hooks";
import { WidgetFunctionComponent, PropsWithChildren, WidgetPropsMap } from "./types";

export const TreeNode: WidgetFunctionComponent<PropsWithChildren & WidgetPropsMap["TreeNode"]> = ({ children, label }) => {
    const widgetRegistratonService = useWidgetRegistrationService();
    const idRef = useRef(widgetRegistratonService.generateId());

    return (
        <widget type="TreeNode" id={idRef.current} label={label}>
            {children}
        </widget>
    );
};
