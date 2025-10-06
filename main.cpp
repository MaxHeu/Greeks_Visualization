#include "data.hpp"
#include "func.hpp"
#include "Greeks.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <matplot/matplot.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

using namespace std;
using namespace matplot;

int main() {
    // ---- Initialize GLFW + ImGui ----
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Option Greeks Control Panel", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    //  parameters 
    Parameters params;
    ReadParameters("../param.txt", params);
    double S0 = params.S0; double T = params.T; double K = params.K; double sigma = params.sigma; double r = params.r; double q = params.q; double ITM = params.ITM; double OTM = params.OTM;
    std::string greekTypes = params.greekTypes; std::string optionTypes = params.optionTypes; int numMaturities = static_cast<int>(params.numMaturities);
    

    vector<double> StockPrices, TimeToMaturities;
    std::vector<std::vector<std::vector<std::vector<double>>>> GreekValues;

    if (Recompute(K, S0, r, q, T, sigma, numMaturities, greekTypes, optionTypes, StockPrices, TimeToMaturities, GreekValues) != 0) {
        std::cerr << "Initial Recompute failed" << std::endl;
    }


    // ---- GUI Loop ----
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Parameter Controls");
        // plot either 2D, 3D or Moneyness based on params.plotTypes -> can't do multiple plots in the same run
        if (params.plotTypes.find("Simple") != std::string::npos){
            Plot2D(ITM, OTM, K, S0, r, q, T, sigma, numMaturities, greekTypes, StockPrices, TimeToMaturities, GreekValues, optionTypes, params.plotTypes);
        } else if (params.plotTypes.find("3D") != std::string::npos){
            Plot3D(ITM, OTM, K, S0, r, q, T, sigma, numMaturities, greekTypes, StockPrices, TimeToMaturities, GreekValues, optionTypes, params.plotTypes);
        } else if (params.plotTypes.find("Moneyness") != std::string::npos){
            PlotMoneyness(ITM, OTM, K, S0, r, q, T, sigma, numMaturities, greekTypes, StockPrices, TimeToMaturities, GreekValues, optionTypes, params.plotTypes);
        } else {
            std::cerr << "Unknown plot type: " << params.plotTypes << ". Defaulting to Simple." << std::endl;
        }
        ImGui::End();

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();



    return 0;
}
