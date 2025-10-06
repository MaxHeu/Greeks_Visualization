#ifndef FUNC_HPP_
#define FUNC_HPP_

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <matplot/matplot.h>
#include "data.hpp"
#include <cstdlib>


double Price(double &K, double &S, double &r, double &T, double &sigma);

// Recompute GreekValues and X/Y grids from parameters
int Recompute(double &K, double &S0, double &r, double &q, double &T, double &sigma, int numMaturities,const std::string &greekTypes, const std::string &optionTypes,
              std::vector<double> &StockPrices, std::vector<double> &TimeToMaturities,
              std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues);

// Plot functions 
void Plot2D(double &ITM,double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma, int &numMaturities,const std::string &GreekTypes,std::vector<double> &X,std::vector<double> &Y,std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,const std::string &optionTypes, const std::string &plotTypes);
void Plot3D(double &ITM,double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma, int &numMaturities,const std::string &GreekTypes,std::vector<double> &X,std::vector<double> &Y,std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,const std::string &optionTypes, const std::string &plotTypes);
// Plot Greeks vs Moneyness for ITM and OTM options -> ITM and OTM are percentages of the strike price and must be integer between 0 and 100
void PlotMoneyness(double &ITM,double &OTM, double &K, double &S0, double &r, double &q, double &T, double &sigma, int &numMaturities,const std::string &GreekTypes,std::vector<double> &X,std::vector<double> &Y,std::vector<std::vector<std::vector<std::vector<double>>>> &GreekValues,const std::string &optionTypes, const std::string &plotTypes);
#endif /* FUNC_HPP_ */