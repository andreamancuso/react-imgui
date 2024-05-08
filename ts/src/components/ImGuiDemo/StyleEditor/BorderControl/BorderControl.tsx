import { useCallback, useState } from "react";
import { ReactImgui } from "src/components/ReactImgui/components";
import { useWidgetRegistrationService } from "src/hooks/useWidgetRegistrationService";

export const BorderControl = () => {
    const service = useWidgetRegistrationService();
    const [windowBorder, setWindowBorder] = useState(true);
    const [frameBorder, setFrameBorder] = useState(false);
    const [popupBorder, setPopupBorder] = useState(true);

    const handleWindowBorderChanged = useCallback((event: any) => {
        console.log(event.nativeEvent.value);

        if (event.nativeEvent) {
            setWindowBorder(event.nativeEvent.value);
        }
    }, []);

    const handleFrameBorderChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setFrameBorder(event.nativeEvent.value);
        }
    }, []);

    const handlePopupBorderChanged = useCallback((event: any) => {
        if (event.nativeEvent) {
            setPopupBorder(event.nativeEvent.value);
        }
    }, []);

    return (
        <ReactImgui.SameLine>
            <ReactImgui.Checkbox
                label="WindowBorder"
                onChange={handleWindowBorderChanged}
                defaultChecked={windowBorder}
            />
            <ReactImgui.UnformattedText text={windowBorder ? "[enabled]" : "[disabled]"} />
            <ReactImgui.Checkbox
                label="FrameBorder"
                onChange={handleFrameBorderChanged}
                defaultChecked={frameBorder}
            />
            <ReactImgui.UnformattedText text={frameBorder ? "[enabled]" : "[disabled]"} />
            <ReactImgui.Checkbox
                label="WindowBorder"
                onChange={handlePopupBorderChanged}
                defaultChecked={popupBorder}
            />
            <ReactImgui.UnformattedText text={popupBorder ? "[enabled]" : "[disabled]"} />
        </ReactImgui.SameLine>
    );
};
