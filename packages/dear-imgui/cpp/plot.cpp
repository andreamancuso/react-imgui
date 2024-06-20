#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "implotview.h"

ImPlotAxisFlags axis_auto_fit_flag = ImPlotAxisFlags_AutoFit;

bool enable_axis_auto_fit_flag = true;

int axisValueFormatter(double value, char* buff, int size, void* decimalPlaces) {
    if (value == 0) {
        return snprintf(buff, size,"0");
    }

    return snprintf(buff, size, "%.*f", reinterpret_cast<int>(decimalPlaces), value);
}

class Plot final {
    private:
        std::vector<double> x_values;
        std::vector<double> y_values;

        int x_axis_decimal_digits = 0;
        int y_axis_decimal_digits = 0;

        int data_points_limit = 6000;

    public:
        Plot() {
            x_values.reserve(10000);
            y_values.reserve(10000);
        }

        void Render(int window_width, int window_height) {
            // ImGui::Text("Received %lu data points, (%d frames per second)", x_values.size(), (int)io.Framerate);
            ImGui::Text("Received %lu data points", x_values.size());
            ImGui::Checkbox("Auto-fit Axes", &enable_axis_auto_fit_flag);
            ImGui::SameLine();

            static int e = 0;
            ImGui::RadioButton("line plot", &e, 0);
            ImGui::SameLine();
            ImGui::RadioButton("scatter plot", &e, 1);

            if (ImPlot::BeginPlot("Line Plots", ImVec2(-1, -1), ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle)) {
                if (enable_axis_auto_fit_flag) {
                    ImPlot::SetupAxes("x","y", axis_auto_fit_flag, axis_auto_fit_flag);
                } else {
                    ImPlot::SetupAxes("x","y");
                    // TODO: compute axes limits?
                    // ImPlot::SetupAxesLimits(0,15000,0,1000);
                }

                if (x_axis_decimal_digits > 0) {
                    ImPlot::SetupAxisFormat(ImAxis_X1, axisValueFormatter, (void*)x_axis_decimal_digits);
                }

                if (y_axis_decimal_digits > 0) {
                    ImPlot::SetupAxisFormat(ImAxis_Y1, axisValueFormatter, (void*)y_axis_decimal_digits);
                }

                double* x_valuesPtr = x_values.data();
                double* y_valuesPtr = y_values.data();

                if (e == 0) {
                    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                    ImPlot::PlotLine("line-plot", x_valuesPtr, y_valuesPtr, x_values.size());
                } else {
                    ImPlot::PlotScatter("scatter-plot", x_valuesPtr, y_valuesPtr, x_values.size());
                }
                ImPlot::EndPlot();
            }
        }

        void AppendData(double x, double y) {
            if (x_values.size() >= data_points_limit) {
                x_values.erase(x_values.begin());
                y_values.erase(y_values.begin());
            }

            x_values.push_back(x);
            y_values.push_back(y);
        }

        void SetAxesDecimalDigits(int x, int y) {
            x_axis_decimal_digits = x;
            y_axis_decimal_digits = y;
        }

        void ResetData() {
            x_values.clear();
            y_values.clear();
        }
};

