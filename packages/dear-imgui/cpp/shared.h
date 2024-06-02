#include <string>
#include <optional>
#include "imgui.h"

#pragma once

#ifndef SHARED_STUFF
#define SHARED_STUFF

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

#endif
