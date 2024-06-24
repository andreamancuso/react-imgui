#include <string>
#include <optional>
#include <sstream>
#include <iomanip>
#include "imgui.h"
#include "shared.h"

#pragma once

ImVec4 RGBAtoIV4(int r, int g, int b, float a) {
	float newr = r / 255.0;
	float newg = g / 255.0;
	float newb = b / 255.0;

	return ImVec4(newr, newg, newb, a);
}
ImVec4 RGBAtoIV4(int r, int g, int b) {
	return RGBAtoIV4(r, g, b, 1.0f);
}

ImVec4 HEXAtoIV4(const char* hex, float a) {
	int r, g, b;
	std::sscanf(hex, "%02x%02x%02x", &r, &g, &b);
	return RGBAtoIV4(r, g, b, a);
}
ImVec4 HEXAtoIV4(const char* hex) {
	return HEXAtoIV4(hex, 1.0f);
}

float charPercentageToFloat(const char* input) {
    float value;
    std::sscanf(input, "%f%%", &value);

    return value;
}

json IV4toJson(ImVec4 imVec4) {
    json j = {
        {"x", imVec4.x},
        {"y", imVec4.y},
        {"z", imVec4.z},
        {"w", imVec4.w}
    };

    return j;
};

json IV4toJsonTuple(ImVec4 imVec4) {
    json j = {
        imVec4.x,
        imVec4.y,
        imVec4.z,
        imVec4.w
    };

    return j;
};

json IV4toJsonRGBATuple(ImVec4 imVec4) {
    json j = {
        (int)(imVec4.x * 255),
        (int)(imVec4.y * 255),
        (int)(imVec4.z * 255),
        imVec4.w
    };

    return j;
};

// todo: refactor
json IV4toJsonHEXATuple(ImVec4 imVec4) {

    auto rAsInt = (int)(imVec4.x * 255);
    auto gAsInt = (int)(imVec4.y * 255);
    auto bAsInt = (int)(imVec4.z * 255);

    std::string h = "#";

    std::ostringstream rSs;
    rSs << std::setfill('0') << std::setw(2) << std::hex << rAsInt;
    h += rSs.str();

    std::ostringstream gSs;
    gSs << std::setfill('0') << std::setw(2) << std::hex << gAsInt;
    h +=  gSs.str();

    std::ostringstream bSs;
    bSs << std::setfill('0') << std::setw(2) << std::hex << bAsInt;
    h += bSs.str();

    json j = { h, imVec4.w };

    return j;
};
