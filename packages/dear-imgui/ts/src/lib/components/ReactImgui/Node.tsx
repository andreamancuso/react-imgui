import { FunctionComponent } from "react";
import { YogaStyle } from "src/lib/stylesheet/yoga-style";
import { PropsWithChildren } from "./types";

type Props = {
    style?: YogaStyle;
};

export const Node: FunctionComponent<PropsWithChildren & Props> = ({ children, style }) => {
    return <node style={style}>{children}</node>;
};
