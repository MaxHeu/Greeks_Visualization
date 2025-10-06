#include "data.hpp"
#include <fstream>
#include <iostream>
#include <string>
void ReadParameters(const std::string &filename, Parameters &params) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string raw_value = line.substr(pos + 1);

            // Trim whitespace
            raw_value.erase(0, raw_value.find_first_not_of(" \t"));
            raw_value.erase(raw_value.find_last_not_of(" \t") + 1);

            if (key == "InitialStock") {
                params.S0 = std::stod(raw_value);
            } else if (key == "MaxMaturity") {
                params.T = std::stod(raw_value);
            } else if (key == "Strike") {
                params.K = std::stod(raw_value);
            } else if (key == "Vol") {
                params.sigma = std::stod(raw_value);
            } else if (key == "RiskFreeRate") {
                params.r = std::stod(raw_value);
            } else if (key == "Yield") {
                params.q = std::stod(raw_value);
            } else if (key == "ITM") {
                params.ITM = std::stod(raw_value);
            } else if (key == "OTM") {
                params.OTM = std::stod(raw_value);
            } else if (key == "NumberOfMaturities") {
                params.numMaturities = std::stod(raw_value);
            } else if (key == "Greeks") {
                params.greekTypes = raw_value; 
            } else if (key == "Options") {
                params.optionTypes = raw_value; 
            } else if (key == "Plots") {
                params.plotTypes = raw_value; 
            } else {
                std::cerr << "Unknown parameter: " << key << std::endl;
            }
        }
    }
    file.close();
}
    