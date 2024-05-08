import { useCallback, useState } from "react";
import { ReactImgui } from "src/components/ReactImgui/components";
import { useWidgetRegistrationService } from "src/hooks/useWidgetRegistrationService";
import { Primitive } from "src/components/ReactImgui/types";

export const ItemInnerSpacing = () => {
    const service = useWidgetRegistrationService();
    const [value, setValue] = useState<[number, number]>([9, 9]);

    const handleValueChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setValue(event.nativeEvent.value);
        }
    }, []);

    return (
        <ReactImgui.SameLine>
            <ReactImgui.MultiSlider
                label="ItemInnerSpacing"
                onChange={handleValueChanged}
                numValues={2}
                min={0}
                max={20}
            />
            {value && <ReactImgui.UnformattedText text={`[${value[0]},${value[1]}]`} />}
        </ReactImgui.SameLine>
    );
};
