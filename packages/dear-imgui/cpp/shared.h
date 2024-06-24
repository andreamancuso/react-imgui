#include <string>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include "imgui.h"

#pragma once

#ifndef SHARED_STUFF
#define SHARED_STUFF

using json = nlohmann::json;

enum HorizontalAlignment
{
    // HorizontalAlignment_None    = -1,
    HorizontalAlignment_Left    = 0,
    HorizontalAlignment_Right   = 1,
    HorizontalAlignment_Center  = 2 // Wondering this will ever be supported the way I'd like
};

typedef std::unordered_map<std::string, std::string> TableRow;
typedef std::vector<TableRow> TableData;

typedef std::variant<std::monostate, ImVec2, float> StyleVarValue;
typedef std::unordered_map<ImGuiCol, ImVec4> StyleColors;
typedef std::unordered_map<ImGuiStyleVar, StyleVarValue> StyleVars;

typedef void (*OnTextChangedCallback)(int id, std::string value);
typedef void (*OnComboChangedCallback)(int id, int value);
typedef void (*OnNumericValueChangedCallback)(int id, int value);
typedef void (*OnBooleanValueChangedCallback)(int id, bool value);
typedef void (*OnMultipleNumericValuesChangedCallback)(int id, float* values, int size);
typedef void (*OnClickCallback)(int id);

ImVec4 RGBAtoIV4(int r, int g, int b, float a);
ImVec4 RGBAtoIV4(int r, int g, int b);

ImVec4 HEXAtoIV4(const char* hex, float a);
ImVec4 HEXAtoIV4(const char* hex);

float charPercentageToFloat(const char* input);

json IV4toJson(ImVec4 imVec4);
json IV4toJsonTuple(ImVec4 imVec4);
json IV4toJsonRGBATuple(ImVec4 imVec4);
json IV4toJsonHEXATuple(ImVec4 imVec4);

#endif
