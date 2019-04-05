/**
 * Twinkle FGS-Sim: AstroUtilities namespace.
 * Utility functions to calculate astronomical quantities
 * 
 * @file astroUtilities.hpp
 * @brief Header file for AstroUtilities namespace
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-04-02
 */

#pragma once
#include "telescopes.hpp"

namespace astroUtilities
{

double meanReceivedPhotons(std::vector<double> mags, std::vector<filter> fltrs, double expT, Telescope tel);
double photonsInBand(std::vector<double> mags, std::vector<filter> fltrs);
double photonsInBand(double mag, struct filter flt);

double airmass(double alt);
double extinctionInMags(double alt, double extinction_coefficient);
double extinctionInPercentage(double alt, double extinction_coefficient);

} // namespace astroUtilities