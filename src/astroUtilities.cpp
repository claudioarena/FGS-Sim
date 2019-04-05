/**
 * Twinkle FGS-Sim: AstroUtilities namespace.
 * Utility functions to calculate astronomical quantities
 * 
 * @file astroUtilities.cpp
 * @brief AstroUtilities namespace, to calculate astronomical quantities
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-04-02
 */
#include <cmath>
#include <vector>

#include "astroUtilities.hpp"
#include "telescopes.h"
#include "typedefs.h" //used for definition of PI

namespace astroUtilities
{
/**
 * function to find the airmass at a given altitude, in degrees.
 * @brief Find the airmass at a given altitude, in degrees
 * @param alt Altitude in degrees above the horizon
 * @return Airmass
 */
double airmass(double alt)
{
    double a = 47 * pow(alt, 1.1);
    double b = alt + (244 / (165 + a));
    double z = 1 / sin(b * (M_PI / 180));
    return z;
}

/**
 * function to find exction amount in magnitudes, at a given altitude.
 * @brief Finds the amount of extinction, in mags, at a given altitude
 * @param alt Altitude, in degrees, above the horizon
 * @extinction_coefficient Extinction coefficient to use
 * @return Extinction amount in magnitudes.
 */
double extinctionInMags(double alt, double extinction_coefficient)
{
    double z = airmass(alt);
    return extinction_coefficient * z;
}

/**
 * function to find exction amount in percentage of left flux, at a given altitude. Return value of 1 is no extinction, 0.5 is half of the flux is removed.
 * @brief Finds the amount of extinction, in percentage of left flux, at a given altitude
 * @param alt Altitude, in degrees, above the horizon
 * @return Extinction amount in percentage reduction.
 */
double extinctionInPercentage(double alt, double extinction_coefficient)
{
    double inMags = extinctionInMags(alt, extinction_coefficient);
    return pow(10, -0.4 * inMags);
}

/**
 * function to find the number of photons per second, received on a telescope sensor
 * @brief Finds the number of photons per second for a given band combination and magnitude. 
 * @param mag Band-magnitudes vector of the star
 * @param band Spectral bands vectors. Elements are filter structs.
 * @return Number of photons from a star in the given bands per second
 */

double meanReceivedPhotons(std::vector<double> mags, std::vector<filter> fltrs, double expT, Telescope tel)
{
    double telescopeArea = M_PI * pow((tel.DIAMETER / 1000.0) / 2.0, 2);
    double efficieny = tel.mirrorEfficiency * tel.CCD_EFFICIENCY;
    double photonFlux = astroUtilities::photonsInBand(mags, fltrs);

    double expected_photons = photonFlux * telescopeArea * (1 - tel.obstruction_area) * efficieny * expT;

    return expected_photons;
    //TODO: gain and efficiency should be considered as probabilities/distributions!
    //TODO: what about GAIN / ADU?
    ///float detectedADU = expected_photons / tel.GAIN;
}

/**
 * function to find the number of photons per second in the sum of multiple bands.
 * @brief Finds the number of photons per second for a given band combination and magnitude. 
 * @param mag Band-magnitudes vector of the star
 * @param band Spectral bands vectors. Elements are filter structs.
 * @return Number of photons from a star in the given bands per second
 */
double photonsInBand(std::vector<double> mags, std::vector<filter> fltrs)
{
    uint8_t nFilters = fltrs.size();
    uint8_t nMags = mags.size();

    if (nMags != nFilters)
    {
        throw("error. Number of specified magnitudes and filters must match");
    }
    if (nFilters < 1)
    {
        throw("error. At least one magnitude must be specified");
    }

    double photons = 0;
    for (int i = 0; i < nFilters; i++)
    {
        photons += photonsInBand(mags[i], fltrs[i]);
    }

    return photons;
}

/**
 * function to find the number of photons per second for a given band-magnitude.
 *
 * @brief Finds the number of photons per second for a given band and magnitude. 
 * @param mag Band-magnitude of the star
 * @param flt Filter struct: B, V, R. 
 * @return Number of photons from a star in the given band per second per m2
 */
double photonsInBand(double mag, struct filter flt)
{
    double m0_photons = (1.51 * (pow(10, 7))) * (flt.zero_point_Jy * (flt.bandWidth / flt.centerBand));

    double power = pow(2.512, -1 * mag);
    double photons = power * m0_photons; // Photons per second, per m-2
    return photons;
}

} // namespace astroUtilities