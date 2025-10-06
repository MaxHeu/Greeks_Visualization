#include "Greeks.hpp"
#include <cmath>
#include <random>


// Normal distribution functions
double norm_pdf(double mu, double sigma, double x) { // N'
    return (std::exp(-0.5 * std::pow((x - mu) / sigma, 2)) / (sigma * std::sqrt(2 * M_PI)));
}

double norm_cdf(double mu, double sigma, double x) { // N
    return 0.5 * (1 + erf((x-mu) / (sigma * sqrt(2))));
}

// d1 and d2 functions
double d1(double &K, double &S, double &r, double &q, double &T, double &sigma) {
    return (log(S / K) + (r - q + 0.5 * pow(sigma, 2)) * T) / (sigma * sqrt(T));
}

double d2(double &K, double &S, double &r, double &q, double &T, double &sigma) {
    return d1(K, S, r, q, T, sigma) - sigma * sqrt(T);
}

// Greeks functions - Analytical formulas via BSM


double Delta(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall) {
    if (isCall) {
        return std::exp(-q * T) * norm_cdf(0.0, 1.0, d1(K, S, r, q, T, sigma));
    } else {
        return std::exp(-q * T) * (norm_cdf(0.0, 1.0, d1(K, S, r, q, T, sigma)) - 1);
    }
}

double Gamma(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall) {
    // Note that "isCall" does not matter for Gamma, as it is the same for calls and puts :)
    return (std::exp(-q * T) * norm_pdf(0.0, 1.0, d1(K, S, r, q, T, sigma))) / (S * sigma * std::sqrt(T));
}

double Theta(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall) {
    if (isCall) {
        return (- (S * sigma * std::exp(-q * T) * norm_pdf(0.0, 1.0, d1(K, S, r, q, T, sigma))) / (2 * std::sqrt(T))
                - r * K * std::exp(-r * T) * norm_cdf(0.0, 1.0, d2(K, S, r, q, T, sigma))
                + q * S * std::exp(-q * T) * norm_cdf(0.0, 1.0, d1(K, S, r, q, T, sigma)));
    } else {
        return (- (S * sigma * std::exp(-q * T) * norm_pdf(0.0, 1.0, d1(K, S, r, q, T, sigma))) / (2 * std::sqrt(T))
                + r * K * std::exp(-r * T) * norm_cdf(0.0, 1.0, -d2(K, S, r, q, T, sigma))
                - q * S * std::exp(-q * T) * norm_cdf(0.0, 1.0, -d1(K, S, r, q, T, sigma)));
    }
}

double Vega(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall) {
    // Note that "isCall" does not matter for Vega, as it is the same for calls and puts :)
    return S * std::exp(-q * T) * norm_pdf(0.0, 1.0, d1(K, S, r, q, T, sigma)) * std::sqrt(T);
}

double Rho(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall) {
    if (isCall) {
        return K * T * std::exp(-r * T) * norm_cdf(0.0, 1.0, d2(K, S, r, q, T, sigma));
    } else {
        return -K * T * std::exp(-r * T) * norm_cdf(0.0, 1.0, -d2(K, S, r, q, T, sigma));
    }
}

double ComputeGreek(std::vector<double> &StockPrices, std::vector<double> &TimeToMaturities,std::string greekTypes, std::string optionTypes, double &K, double &S, double &r, double &q, double &T, double &sigma, std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues) {
    /*
    Input :
    greekTypes : string containing the types of Greeks to compute (e.g., "Delta,Gamma,Vega")
    optionTypes : string containing the types of options to compute (e.g., "Call,Put")
    K : Strike price
    S : Stock price
    r : Risk-free interest rate
    q : Dividend yield
    T : Time to maturity
    sigma : Volatility
    GreekValues : 4D vector to store computed Greek values [Number of Greeks][Number of Options][StockPrices.size()][TimeToMaturities.size()]

    Output : Filled in GreekValues matrix 
    */
   using namespace std;

    // check if we need to compute call and/or put
    bool computeCall = (optionTypes.find("Call") != std::string::npos);
    bool computePut = (optionTypes.find("Put") != std::string::npos);
    

    // Fill in GreekValues 4D vector
    // order for greek types : Delta, Gamma, Vega, Theta, Rho
    // order for option types : Call, Put
    size_t greekIndex = 0;
    if (greekTypes.find("Delta") != std::string::npos) {
        size_t optionIndex = 0;
        if (computeCall) {
            // Fill in Call Delta values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Delta(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, true); // true for Call
                }
            }
            optionIndex++;
        }
        if (computePut) {
            // Fill in Put Delta values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Delta(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, false); // false for Put
                }
            }
        }
        greekIndex++;
    } if (greekTypes.find("Gamma") != std::string::npos) {
        size_t optionIndex = 0;
        if (computeCall) {
            // Fill in Call Gamma values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Gamma(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, true); // true for Call
                }
            }
            optionIndex++;
        }
        if (computePut) {
            // Fill in Put Gamma values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Gamma(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, false); // false for Put
                }
            }
        }
        greekIndex++;
    } if (greekTypes.find("Vega") != std::string::npos) {
        size_t optionIndex = 0;
        if (computeCall) {
            // Fill in Call Vega values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Vega(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, true); // true for Call
                }
            }
            optionIndex++;
        }
        if (computePut) {
            // Fill in Put Vega values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Vega(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, false); // false for Put
                }
            }
        }
        greekIndex++;
    } if (greekTypes.find("Theta") != std::string::npos) {
        size_t optionIndex = 0;
        if (computeCall) {
            // Fill in Call Theta values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Theta(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, true); // true for Call
                }
            }
            optionIndex++;
        }
        if (computePut) {
            // Fill in Put Theta values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Theta(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, false); // false for Put
                }
            }
        }
        greekIndex++;
    } if (greekTypes.find("Rho") != std::string::npos) {
        size_t optionIndex = 0;
        if (computeCall) {
            // Fill in Call Rho values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Rho(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, true); // true for Call
                }
            }
            optionIndex++;
        }
        if (computePut) {
            // Fill in Put Rho values
            for (size_t i = 0; i < StockPrices.size(); ++i) {
                for (size_t j = 0; j < TimeToMaturities.size(); ++j) {
                    GreekValues[greekIndex][optionIndex][i][j] = Rho(K, StockPrices[i], r, q, TimeToMaturities[j], sigma, false); // false for Put
                }
            }
        }
        greekIndex++;
    }

    return 0;
}
