import { useCallback, useState } from "react";
import { ReactImgui } from "src/components/ReactImgui/components";
import { useWidgetRegistrationService } from "src/hooks/useWidgetRegistrationService";
import { Primitive } from "src/components/ReactImgui/types";

export const FramePadding = () => {
    const service = useWidgetRegistrationService();
    const [value, setValue] = useState<[number, number]>([9, 9]);

    // todo: remove casting
    const handleValueChanged = useCallback((values: Primitive[]) => {
        setValue([values[0] as number, values[1] as number]);
    }, []);

    return (
        <ReactImgui.SameLine>
            <ReactImgui.MultiSlider
                label="FramePadding"
                onChange={handleValueChanged}
                numValues={2}
                min={0}
                max={20}
            />
            {value && <ReactImgui.UnformattedText text={`[${value[0]},${value[1]}]`} />}
        </ReactImgui.SameLine>
    );
};
