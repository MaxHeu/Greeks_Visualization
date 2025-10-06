#ifndef GREEKS_HPP_
#define GREEKS_HPP_

#include <cmath>
#include <random>

// Normal distribution functions
double norm_pdf(double mu, double sigma, double x);
double norm_cdf(double mu, double sigma, double x);

// d1 and d2 functions
double d1(double &K, double &S, double &r, double &q, double &T, double &sigma);
double d2(double &K, double &S, double &r, double &q, double &T, double &sigma);

// Greeks functions - Analytical formulas via BSM

double Delta(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall);
double Gamma(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall);
double Theta(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall);
double Vega(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall);
double Rho(double &K, double &S, double &r, double &q, double &T, double &sigma, bool isCall);


double ComputeGreek(std::vector<double> &StockPrices, std::vector<double> &TimeToMaturities,std::string greekTypes, std::string optionTypes, double &K, double &S, double &r, double &q, double &T, double &sigma, std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues);

#endif /* GREEKS_HPP_ */