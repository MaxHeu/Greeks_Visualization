#ifndef DATA_HPP_
#define DATA_HPP_

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
struct Parameters {
    double S0;     // Initial Stock Price
    double T;      // Time to Maturity
    double K;      // Strike Price
    double sigma;  // Volatility
    double r;      // Risk-Free Rate
    double q;      // Dividend Yield
    double ITM;    // In-The-Money percentage
    double OTM;    // Out-Of-The-Money percentage
    std::string optionTypes; // "Call" and/or "Put"
    std::string greekTypes;  // "Delta", "Gamma", "Vega", "Theta", "Rho"
    std::string plotTypes;  // "Simple" -> plot greek versus stock prices, "3D" -> plot greek versus stock prices and maturities, "Moneyness" -> plot greek for ITM,OTM,ATM
    double numMaturities; // Number of maturities
};

void ReadParameters(const std::string& filename, Parameters& params);



#endif /* DATA_HPP_ */