#include "func.hpp"
#include "Greeks.hpp"
#include "imgui.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <matplot/matplot.h>
using namespace matplot;

// Helper: clamp value between lo and hi
template <typename T>
T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi ? hi : v);
}



// Helper: split comma-separated string
std::vector<std::string> split(const std::string &s, char delimiter = ',') {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) tokens.push_back(item);
    }
    return tokens;
}


int Recompute(double &K, double &S0, double &r, double &q, double &T, double &sigma, int numMaturities,const std::string &greekTypes, const std::string &optionTypes,
              std::vector<double> &StockPrices, std::vector<double> &TimeToMaturities,
              std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues) {

    StockPrices.clear();
    TimeToMaturities.clear();


    double S_start = 0, S_end = 2 * K, S_step = K / 100.0;
    for (double S = S_start; S <= S_end; S += S_step)
        StockPrices.push_back(S);

    double T_start = 0.01, T_end = T, T_step = (T_end - T_start) / static_cast<double>(numMaturities);
    for (double t = T_start; t <= T_end; t += T_step)
        TimeToMaturities.push_back(t);

    size_t countGreeks = std::count(greekTypes.begin(), greekTypes.end(), ',') + 1;
    size_t countOptions = std::count(optionTypes.begin(), optionTypes.end(), ',') + 1;

    GreekValues.resize(countGreeks,
        std::vector<std::vector<std::vector<double>>>(countOptions,
            std::vector<std::vector<double>>(StockPrices.size(),
                std::vector<double>(TimeToMaturities.size()))));

    if (ComputeGreek(StockPrices, TimeToMaturities, greekTypes, optionTypes, K, S0, r, q, T, sigma, GreekValues) != 0) {
        ComputeGreek(StockPrices, TimeToMaturities, greekTypes, optionTypes, K, S0, r, q, T, sigma, GreekValues);
        std::cerr << "Error in ComputeGreek function." << std::endl;
        return -1;
    }
    return 0;
}


void Plot2D(double &ITM, double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma,
            int &numMaturities, const std::string &GreekTypes, std::vector<double> &X, std::vector<double> &Y,
            std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,
            const std::string &optionTypes, const std::string &plotTypes) {

    bool changed = false;

    // parameter ranges
    double minS0 = 00.0,  maxS0 = 999.0;
    double minK  = 00.0,  maxK  = 999.0;
    double minT  = 0.1,   maxT  = 5.0;
    double minR  = 0.0,   maxR  = 0.3;
    double minSigma = 0.05, maxSigma = 1.0;
    double minQ  = 0.0,   maxQ  = 0.3;
    double minITM = 0.0,  maxITM = 99.0;
    double minOTM = 0.0,  maxOTM = 99.0;

    // sliders
    changed |= ImGui::SliderScalar("S0",    ImGuiDataType_Double, &S0,    &minS0,    &maxS0,    "%.2f");
    changed |= ImGui::SliderScalar("K",     ImGuiDataType_Double, &K,     &minK,     &maxK,     "%.2f");
    changed |= ImGui::SliderScalar("T",     ImGuiDataType_Double, &T,     &minT,     &maxT,     "%.2f");
    changed |= ImGui::SliderScalar("r",     ImGuiDataType_Double, &r,     &minR,     &maxR,     "%.4f");
    changed |= ImGui::SliderScalar("sigma", ImGuiDataType_Double, &sigma, &minSigma, &maxSigma, "%.3f");
    changed |= ImGui::SliderScalar("q",     ImGuiDataType_Double, &q,     &minQ,     &maxQ,     "%.4f");
    changed |= ImGui::SliderInt("Number of maturities", &numMaturities, 1, 50);
    changed |= ImGui::SliderScalar("ITM (%)", ImGuiDataType_Double, &ITM, &minITM, &maxITM, "%.2f");
    changed |= ImGui::SliderScalar("OTM (%)", ImGuiDataType_Double, &OTM, &minOTM, &maxOTM, "%.2f");

    bool doShow = false;

    if (ImGui::Button("Recompute Greeks")) {
        if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0)
            doShow = true;
    }

        static bool pending_recompute = false;
        if (changed) {
            pending_recompute = true;
        }
        if (pending_recompute && !ImGui::IsAnyItemActive()) {
            if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0) doShow = true;
            pending_recompute = false;
        }

    // Display current parameter values
    ImGui::Text("Current parameters:");
    ImGui::Text("S0=%.2f  K=%.2f  T=%.2f", S0, K, T);
    ImGui::Text("sigma=%.2f  r=%.2f  q=%.2f", sigma, r, q);

    // Skip plotting if recomputation not requested
    if (!doShow) return;

    // plots
    auto greeks  = split(GreekTypes);
    auto options = split(optionTypes);

    size_t gCount = greeks.size();
    size_t oCount = options.size();

    // Determine subplot layout
    int rows = 1, cols = 1;
    if (gCount == 1) { rows = 1; cols = 1; }
    else if (gCount == 2) { rows = 1; cols = 2; }
    else if (gCount == 3) { rows = 1; cols = 3; }
    else if (gCount == 4) { rows = 2; cols = 2; }
    else if (gCount <= 6) { rows = 2; cols = 3; }
    else if (gCount <= 9) { rows = 3; cols = 3; }
    else { rows = (gCount + 2) / 3; cols = 3; }

    figure_handle f = figure(true);
    f->size(1200, 800);

    // Plot all Greeks for all option types
    for (size_t g = 0; g < gCount; g++) {
        auto ax = f->add_subplot(rows, cols, g + 1);

        for (size_t o = 0; o < oCount; o++) {
            bool isCall = (options[o] == "Call" || options[o] == "call");
            std::array<float, 3> base = isCall ? std::array<float, 3>{0.f, 0.f, 1.f}  // blue
                                               : std::array<float, 3>{1.f, 0.f, 0.f}; // red

            ax->colororder_index(0);

            for (size_t j = 1; j < Y.size(); j++) {
                std::vector<double> Z;
                Z.reserve(X.size());
                for (size_t i = 0; i < X.size(); i++) {
                    if (i < GreekValues[g][o].size() && j < GreekValues[g][o][i].size()) {
                        Z.push_back(GreekValues[g][o][i][j]);
                    } else {
                        Z.push_back(0.0);
                    }
                }
                // Fading colors
                float fade = (Y.size() > 1) ?
                                 static_cast<float>(j) / static_cast<float>(Y.size() - 1)
                                 : 0.f;

                std::array<float, 3> faded_color = {
                    base[0] - (1.f - base[0]) * fade,
                    base[1] - (1.f - base[1]) * fade,
                    base[2] - (1.f - base[2]) * fade
                };

                auto line = ax->plot(X, Z);
                line->line_width(2);
                line->color({0.5f, faded_color[0], faded_color[1], faded_color[2]});
                std::string leg = "T=" + std::to_string(clamp(Y[j], 0.01, 99.0)) + " " + options[o];
                line->display_name(leg);
                ax->hold(on);
            }
        }

        ax->title(greeks[g]);
        ax->xlabel("Stock Price (S0)");
        ax->ylabel(greeks[g]);
        ax->legend();
        ax->grid(true);
    }

    f->draw();
}

void Plot3D(double &ITM, double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma,
            int &numMaturities, const std::string &GreekTypes, std::vector<double> &X, std::vector<double> &Y,
            std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,
            const std::string &optionTypes, const std::string &plotTypes) {

    bool changed = false;

    // parameter ranges
    double minS0 = 00.0,  maxS0 = 999.0;
    double minK  = 00.0,  maxK  = 999.0;
    double minT  = 0.1,   maxT  = 5.0;
    double minR  = 0.0,   maxR  = 0.3;
    double minSigma = 0.05, maxSigma = 1.0;
    double minQ  = 0.0,   maxQ  = 0.3;
    double minITM = 0.0,  maxITM = 99.0;
    double minOTM = 0.0,  maxOTM = 99.0;

    // sliders
    changed |= ImGui::SliderScalar("S0",    ImGuiDataType_Double, &S0,    &minS0,    &maxS0,    "%.2f");
    changed |= ImGui::SliderScalar("K",     ImGuiDataType_Double, &K,     &minK,     &maxK,     "%.2f");
    changed |= ImGui::SliderScalar("T",     ImGuiDataType_Double, &T,     &minT,     &maxT,     "%.2f");
    changed |= ImGui::SliderScalar("r",     ImGuiDataType_Double, &r,     &minR,     &maxR,     "%.4f");
    changed |= ImGui::SliderScalar("sigma", ImGuiDataType_Double, &sigma, &minSigma, &maxSigma, "%.3f");
    changed |= ImGui::SliderScalar("q",     ImGuiDataType_Double, &q,     &minQ,     &maxQ,     "%.4f");
    changed |= ImGui::SliderInt("Number of maturities", &numMaturities, 1, 50);
    changed |= ImGui::SliderScalar("ITM (%)", ImGuiDataType_Double, &ITM, &minITM, &maxITM, "%.2f");
    changed |= ImGui::SliderScalar("OTM (%)", ImGuiDataType_Double, &OTM, &minOTM, &maxOTM, "%.2f");

    bool doShow = false;

    // manual recompute
    if (ImGui::Button("Recompute Greeks")) {
        if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0)
            doShow = true;
    }

    // automatic recompute
    static bool pending_recompute = false;
    if (changed) {
        pending_recompute = true;
    }
    if (pending_recompute && !ImGui::IsAnyItemActive()) {
        if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0)
            doShow = true;
        pending_recompute = false;
    }

    // parameters display
    ImGui::Text("Current parameters:");
    ImGui::Text("S0=%.2f  K=%.2f  T=%.2f", S0, K, T);
    ImGui::Text("sigma=%.2f  r=%.2f  q=%.2f", sigma, r, q);

    if (!doShow) return; 

    // plot section
    auto greeks  = split(GreekTypes);
    auto options = split(optionTypes);

    size_t gCount = greeks.size();
    size_t oCount = options.size();

    // Determine subplot layout
    int rows = 1, cols = 1;
    if (gCount == 1) { rows = 1; cols = 1; }
    else if (gCount == 2) { rows = 1; cols = 2; }
    else if (gCount == 3) { rows = 1; cols = 3; }
    else if (gCount == 4) { rows = 2; cols = 2; }
    else if (gCount <= 6) { rows = 2; cols = 3; }
    else if (gCount <= 9) { rows = 3; cols = 3; }
    else { rows = (gCount + 2) / 3; cols = 3; }

    figure_handle f = figure(true);
    f->size(1200, 800);

    // Greek and options loops
    for (size_t g = 0; g < gCount; g++) {
        auto ax = f->add_subplot(rows, cols, g + 1, true);
        ax->hold(on);

        for (size_t o = 0; o < oCount; o++) {
            bool isCall = (options[o] == "Call" || options[o] == "call");
            std::array<float,3> base = isCall ? std::array<float,3>{0.f,0.f,1.f}  // blue
                                              : std::array<float,3>{1.f,0.f,0.f}; // red

            // meshgird X,Y
            std::vector<std::vector<double>> x(X.size(), std::vector<double>(Y.size()));
            std::vector<std::vector<double>> y(X.size(), std::vector<double>(Y.size()));
            std::vector<std::vector<double>> z(X.size(), std::vector<double>(Y.size()));

            for (size_t i = 0; i < X.size(); i++) {
                for (size_t j = 0; j < Y.size(); j++) {
                    x[i][j] = X[i];
                    y[i][j] = Y[j];
                    if (i < GreekValues[g][o].size() && j < GreekValues[g][o][i].size())
                        z[i][j] = GreekValues[g][o][i][j];
                    else
                        z[i][j] = 0.0;
                }
            }

            // plot surface
            auto s = ax->surf(x, y, z);
            s->edge_color("none"); 
            s->face_alpha(0.8);
            //s->color({base[0], base[1], base[2]});
        }

        ax->xlabel("Stock Price (S0)");
        ax->ylabel("Time to Maturity (T)");
        ax->zlabel(greeks[g]);
        ax->title(greeks[g]);
        ax->grid(true);
    }

    f->draw();
}


void PlotMoneyness(double &ITM, double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma,
                   int &numMaturities, const std::string &GreekTypes, std::vector<double> &X, std::vector<double> &Y,
                   std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,
                   const std::string &optionTypes, const std::string &plotTypes) {

    bool changed = false;

    // parameter ranges
    double minS0 = 00.0,  maxS0 = 999.0;
    double minK  = 00.0,  maxK  = 999.0;
    double minT  = 0.1,   maxT  = 5.0;
    double minR  = 0.0,   maxR  = 0.3;
    double minSigma = 0.05, maxSigma = 1.0;
    double minQ  = 0.0,   maxQ  = 0.3;
    double minITM = 0.0,  maxITM = 99.0;
    double minOTM = 0.0,  maxOTM = 99.0;

    // sliders
    changed |= ImGui::SliderScalar("S0",    ImGuiDataType_Double, &S0,    &minS0,    &maxS0,    "%.2f");
    changed |= ImGui::SliderScalar("K",     ImGuiDataType_Double, &K,     &minK,     &maxK,     "%.2f");
    changed |= ImGui::SliderScalar("T",     ImGuiDataType_Double, &T,     &minT,     &maxT,     "%.2f");
    changed |= ImGui::SliderScalar("r",     ImGuiDataType_Double, &r,     &minR,     &maxR,     "%.4f");
    changed |= ImGui::SliderScalar("sigma", ImGuiDataType_Double, &sigma, &minSigma, &maxSigma, "%.3f");
    changed |= ImGui::SliderScalar("q",     ImGuiDataType_Double, &q,     &minQ,     &maxQ,     "%.4f");
    changed |= ImGui::SliderInt("Number of maturities", &numMaturities, 1, 50);
    changed |= ImGui::SliderScalar("ITM (%)", ImGuiDataType_Double, &ITM, &minITM, &maxITM, "%.2f");
    changed |= ImGui::SliderScalar("OTM (%)", ImGuiDataType_Double, &OTM, &minOTM, &maxOTM, "%.2f");

    bool doShow = false;

    // manual recompute
    if (ImGui::Button("Recompute Greeks")) {
        if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0)
            doShow = true;
    }

    // automatic recompute
    static bool pending_recompute = false;
    if (changed) {
        pending_recompute = true;
    }
    if (pending_recompute && !ImGui::IsAnyItemActive()) {
        if (Recompute(K, S0, r, q, T, sigma, numMaturities, GreekTypes, optionTypes, X, Y, GreekValues) == 0)
            doShow = true;
        pending_recompute = false;
    }

    // Display parameters values
    ImGui::Text("Current parameters:");
    ImGui::Text("S0=%.2f  K=%.2f  T=%.2f", S0, K, T);
    ImGui::Text("sigma=%.2f  r=%.2f  q=%.2f", sigma, r, q);

    if (!doShow) return; 

    //Plots
    auto greeks  = split(GreekTypes);
    auto options = split(optionTypes);

    size_t gCount = greeks.size();
    size_t oCount = options.size();

    // Subplot layout
    int rows = 1, cols = 1;
    if (gCount == 1) { rows = 1; cols = 1; }
    else if (gCount == 2) { rows = 1; cols = 2; }
    else if (gCount == 3) { rows = 1; cols = 3; }
    else if (gCount == 4) { rows = 2; cols = 2; }
    else if (gCount <= 6) { rows = 2; cols = 3; }
    else if (gCount <= 9) { rows = 3; cols = 3; }
    else { rows = (gCount + 2) / 3; cols = 3; }

    figure_handle f = figure(true);
    f->size(1200, 800);

    // Greek loop
    for (size_t g = 0; g < gCount; g++) {
        auto ax = f->add_subplot(rows, cols, g + 1);
        ax->hold(on);

        for (size_t o = 0; o < oCount; o++) {
            bool isCall = (options[o] == "Call" || options[o] == "call");
            std::array<float, 3> base = isCall ? std::array<float, 3>{0.f, 0.f, 1.f}  // blue
                                               : std::array<float, 3>{1.f, 0.f, 0.f}; // red

            // Compute approximate index for moneyness
            int idxATM = (int)std::round((X.size() - 1) / 2.0); // middle index = ATM
            int idxITM = idxATM;
            int idxOTM = idxATM;

            // Adjust for ITM / OTM relative to ATM
            double step = (X.size() - 1) / 100.0;
            if (isCall) {
                idxITM = clamp((int)(idxATM + ITM * step), 0, (int)X.size() - 1);
                idxOTM = clamp((int)(idxATM - OTM * step), 0, (int)X.size() - 1);
            } else {
                idxITM = clamp((int)(idxATM - ITM * step), 0, (int)X.size() - 1);
                idxOTM = clamp((int)(idxATM + OTM * step), 0, (int)X.size() - 1);
            }

            // Extract Greek values
            std::vector<double> GreekITM(Y.size()), GreekOTM(Y.size()), GreekATM(Y.size());
            for (size_t j = 0; j < Y.size(); ++j) {
                GreekITM[j] = GreekValues[g][o][X.size() - idxITM][j];
                GreekOTM[j] = GreekValues[g][o][X.size() - idxOTM][j];
                GreekATM[j] = GreekValues[g][o][idxATM][j];
            }

            // Plot curves
            std::vector<std::string> legends;
            legends.reserve(3);
            auto l1 = ax->plot(Y, GreekITM);
            std::string name1 = "ITM (" + std::to_string((int)ITM) + "%)-" + std::string(!isCall ? " Call" : " Put");
            l1->line_width(2); l1->color(base); l1->display_name(name1  );
            legends.push_back(name1);
            //l1->legend_string("ITM (" + std::to_string((int)ITM) + "%)-" + std::string(isCall ? " Call" : " Put"));

            auto l2 = ax->plot(Y, GreekATM);
            std::string name2 = "ATM-" + std::string(isCall ? " Call" : " Put");
            legends.push_back(name2);
            l2->line_width(2); l2->color({0.3f, 0.8f, 0.3f}); l2->display_name("ATM");
            //l2->legend_string("ATM-" + std::string(isCall ? " Call" : " Put"));

            auto l3 = ax->plot(Y, GreekOTM);
            std::string name3 = "OTM (" + std::to_string((int)OTM) + "%)-" + std::string(isCall ? " Call" : " Put");
            legends.push_back(name3);
            l3->line_width(2); l3->color(base); l3->display_name("OTM (" + std::to_string((int)OTM) + "%)");
            //l3->legend_string("OTM (" + std::to_string((int)OTM) + "%)-" + std::string(isCall ? " Call" : " Put"));
            //ax->hold(on);
            ax->legend(legends);
            //ax->legend();
        }

        ax->xlabel("Time to Maturity");
        ax->ylabel("Greek Value");
        ax->title(greeks[g]);
        //ax->legend();
        ax->grid(true);
    }

    f->draw();
}
