import { MainComponent } from "../ReactImgui";
import { attachSubComponents } from "../../attachSubComponents";
import { BulletText } from "./BulletText";
import { Button } from "./Button";
import { Checkbox } from "./Checkbox";
import { CollapsingHeader } from "./CollapsingHeader";
import { Combo } from "./Combo";
import { Fragment } from "./Fragment";
import { Indent } from "./Indent";
import { InputText } from "./InputText";
import { ItemTooltip } from "./ItemTooltip";
import { MultiSlider } from "./MultiSlider";
import { SameLine } from "./SameLine";
import { Separator } from "./Separator";
import { SeparatorText } from "./SeparatorText";
import { Slider } from "./Slider";
import { TabBar } from "./TabBar";
import { TabItem } from "./TabItem";
import { DisabledText } from "./DisabledText";
import { TextWrap } from "./TextWrap";
import { TreeNode } from "./TreeNode";
import { UnformattedText } from "./UnformattedText";
import { Unknown } from "./Unknown";
import { Table } from "./Table";

export const components = {
    UnformattedText,
    SameLine,
    Unknown,
    InputText,
    CollapsingHeader,
    TreeNode,
    SeparatorText,
    BulletText,
    Indent,
    DisabledText,
    ItemTooltip,
    TextWrap,
    Combo,
    Slider,
    MultiSlider,
    Checkbox,
    Button,
    Separator,
    Fragment,
    TabBar,
    TabItem,
    Table
} as const;

export const ReactImgui = attachSubComponents("ReactImgui", MainComponent, components);
