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
#include "telescopes.h"
#include "typedefs.h"

namespace astroUtilities
{

double meanReceivedPhotons(std::vector<double> mags, std::vector<filter> fltrs, double expT, Telescope tel);
double meanReceivedADUs(std::vector<double> mags, std::vector<filter> fltrs, double expT, Telescope tel);
double photonsInBand(std::vector<double> mags, std::vector<filter> fltrs);
double photonsInBand(double mag, struct filter flt);

double airmass(double alt);
double extinctionInMags(double alt, double extinction_coefficient);
double extinctionInPercentage(double alt, double extinction_coefficient);

pixel_coordinates frameCenter(uint16_t width, uint16_t heigth);
double obstructionPercentage(double telescopeDiameter, double secondaryDiameter);
double reflectionEfficiency(double coatingReflectivity, uint8_t NMirrors);
//TODO: non obsctructed percentage
//TODO: testing photons in band

/**
 * Template utiliy method to calculate average of a vector
 * @brief Calculates the average of numbers held in a vector
 * @param in A vector<> containing the numbers whose average is to be found
 * @return Average number
 */
template <typename T>
inline double const Average(std::vector<T> const &vector)
{
    auto sum = accumulate(vector.begin(), vector.end(), (T)0);
    return ((double)sum / vector.size());
}

/**
 *  Template utility method to calculate the standard deviation of a vector
 * @brief Calculates the standard deviation of numbers held in a vector
 * @param in A vector<> containing the numbers whose SD is to be found
 * @return Standard deviation
 */
template <typename T>
inline double const StDev(std::vector<T> const &vector)
{
    double mean = Average(vector);
    double accum = 0.0;
    for_each(begin(vector), end(vector), [&](const float d) { accum += (d - mean) * (d - mean); });

    return sqrt(accum / (vector.size() - 1));
}

} // namespace astroUtilities