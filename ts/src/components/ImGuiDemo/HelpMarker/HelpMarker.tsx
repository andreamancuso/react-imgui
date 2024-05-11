import { ReactImgui } from "src/lib/components/ReactImgui/components";

type HelpMarkerProps = {
    text: string;
};

export const HelpMarker = ({ text }: HelpMarkerProps) => (
    <ReactImgui.Fragment>
        <ReactImgui.DisabledText text="(?)" />
        <ReactImgui.ItemTooltip>
            <ReactImgui.TextWrap width={35 * 12}>
                <ReactImgui.UnformattedText text={text} />
            </ReactImgui.TextWrap>
        </ReactImgui.ItemTooltip>
    </ReactImgui.Fragment>
);
