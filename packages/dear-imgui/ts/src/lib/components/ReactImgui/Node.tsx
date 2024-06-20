import { FunctionComponent } from "react";
import { YogaStyle } from "src/lib/stylesheet/yoga-style";
import { PropsWithChildren } from "./types";

type Props = {
    root?: boolean;
    style?: YogaStyle;
};

export const Node: FunctionComponent<PropsWithChildren & Props> = ({ root, children, style }) => {
    return (
        <node root={root} style={style}>
            {children}
        </node>
    );
};
