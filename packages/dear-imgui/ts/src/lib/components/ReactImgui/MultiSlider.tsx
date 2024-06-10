import { useRef } from "react";
import { useWidgetRegistrationService } from "../../hooks";
import { WidgetFunctionComponent, WidgetPropsMap } from "./types";

export const MultiSlider: WidgetFunctionComponent<WidgetPropsMap["MultiSlider"]> = ({
    label,
    min,
    max,
    onChange,
    defaultValues,
    numValues,
    decimalDigits,
    style,
}) => {
    const widgetRegistratonService = useWidgetRegistrationService();
    const idRef = useRef(widgetRegistratonService.generateId());

    if (Array.isArray(defaultValues) && defaultValues.length !== numValues) {
        // todo: Mismatch! What to do?
    }

    return (
        <widget
            type="MultiSlider"
            label={label}
            id={idRef.current}
            defaultValues={defaultValues}
            min={min}
            max={max}
            decimalDigits={decimalDigits}
            numValues={numValues}
            onChange={onChange}
            style={style}
        />
    );
};
