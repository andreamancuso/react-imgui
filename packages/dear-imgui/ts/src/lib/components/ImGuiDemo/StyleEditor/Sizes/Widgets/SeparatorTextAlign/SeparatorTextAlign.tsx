import { useCallback, useState } from "react";
import { ReactImgui } from "../../../../../../components/ReactImgui/components";

export const SeparatorTextAlign = () => {
    const [value, setValue] = useState<[number, number]>([0, 0]);

    const handleValueChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setValue([event.nativeEvent.values[0], event.nativeEvent.values[1]]);
        }
    }, []);

    return (
        <ReactImgui.SameLine>
            <ReactImgui.MultiSlider
                label="SeparatorTextAlign"
                onChange={handleValueChanged}
                numValues={2}
                decimalDigits={2}
                min={0}
                max={1}
            />
            {value && <ReactImgui.UnformattedText text={`[${value[0]},${value[1]}]`} />}
        </ReactImgui.SameLine>
    );
};
