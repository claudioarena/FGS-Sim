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
    double totReflectivity = astroUtilities::reflectionEfficiency(tel.COATING_REFLECTIVITY, tel.N_MIRRORS_TO_CAMERA);
    double efficieny = totReflectivity * tel.CCD_EFFICIENCY;
    double photonFlux = astroUtilities::photonsInBand(mags, fltrs);

    double obstructionPercentage = astroUtilities::obstructionPercentage(tel.DIAMETER, tel.SECONDARY_DIAMETER);
    double expected_photons = photonFlux * telescopeArea * (1 - obstructionPercentage) * efficieny * expT;

    return expected_photons;
    //TODO: gain and efficiency should be considered as probabilities/distributions!
    //TODO: what about GAIN / ADU?
    ///float detectedADU = expected_photons / tel.GAIN;
}

/**
 * function to find the number of ADUs per second, received on a telescope sensor
 * @brief Finds the number of photons per second for a given band combination and magnitude. 
 * @param mag Band-magnitudes vector of the star
 * @param band Spectral bands vectors. Elements are filter structs.
 * @return Number of photons from a star in the given bands per second
 */

double meanReceivedADUs(std::vector<double> mags, std::vector<filter> fltrs, double expT, Telescope tel)
{
    return meanReceivedPhotons(mags, fltrs, expT, tel) / tel.GAIN;
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

/*
double zodiacalNoise()
{

    int zodiacalPhotons = 0;
    if (zodiacal == true)
        zodiacalPhotons += (180. / this->horizPixels) * (180. / this->vertPixels) * (astroUtilities::photonsInBand(22.37, B_filter) + astroUtilities::photonsInBand(21.89, V_filter)); // Background photons from zodiacal light. See line 84 of ../README.md.
                                                                                                                                                                                       //float dark = darkSignal * exp(5E-7 * 6.63E-34 * ((1./248)-(1./temperature)) / 1.38E-23); // Dark current variance for 500nm from Meyer-Neidel
}
*/

/**
 * Utility function to calculate the dark signal at the operating temperature given the 0 C dark noise specified by the telescope
 *
 * @brief Calculates dark noise at operating temperature
 * @param tel Telescope used for calculation. Dark noise figure at 0 C and temperature will be used for calculations
 * @return Dark noise value, given as e-/sec/pixel
 */
double darkSignal(Telescope tel)
{
    double referenceDark = tel.DARK_NOISE / 0.165917;                                    //Needed to get dark at 293K
    double temperature = tel.FGS_CCD_TEMP;                                               //Kelvin
    double dark = referenceDark * 122 * pow(temperature, 3) * exp(-6400. / temperature); // Dark current variation with temperature from E2V CCD230-42 datasheet.
    return dark;
}

/**
 * Static function to calculate the number of infrared photons emitted by the mirrors through thermal emission.
 *
 * @brief Calculates noise from mirror infrared emission
 * @param tel Telescope used for calculation. Area of primary mirror and its temperature will be used
 * @return Number of infrared photons emitted from mirror per second
 */
/* 
int mirrorThermalNoise(Telescope tel)
{

    float power = area * Twinkle.emiss * SB_CONST * pow(temperature, 4);
    float wavelength = WIEN / temperature;
    return power / (PLANCK * 3E8 / wavelength);
}
*/

/**
 * function returning the coordinates of the center of the frame.
 *
 * @brief Finds the coordinates of the center of the frame
 * @param heigth The Heigth of the frame
 * @param width The Width of the frame
 * @return Coordinates of the center of the frame, returned as a pixel_coordinates struct
 */
pixel_coordinates frameCenter(uint16_t width, uint16_t heigth)
{
    pixel_coordinates center = {(width / 2.0) - 0.5, (heigth / 2.0) - 0.5};
    return center;
};

/**
 * function returning the obstruction, as a percentage of telescope opening area
 *
 * @brief Finds the obsctruction of the telescope
 * @param telescopeDiameter Diameter of the telescope, in mm
 * @param secondaryDiameter Diameter of the secondary mirror, in mm. Assumes projection of secondary on the aperture is circular
 * @return Obstruction area, in percentage from 0 to 1 (1: completely obstructed)
 */
double obstructionPercentage(double telescopeDiameter, double secondaryDiameter)
{
    double obs_area = pow(secondaryDiameter, 2) / pow(telescopeDiameter, 2);
    return obs_area;
}

/**
 * function returning the total reflection efficiency of the telescope.
 *
 * @brief Finds the efficiency of all combined reflections in the telescope
 * @param coatingReflectivity Coating efficiency from 0 to 1. 1 is a perfect mirror.
 * @param NMirrors Number of mirrors the beam reflects on before reaching the camera
 * @return Reflective efficiency, in percentage from 0 to 1. 1: all light reflected
 */
double reflectionEfficiency(double coatingReflectivity, uint8_t NMirrors)
{
    double totReflectivity = pow(coatingReflectivity, NMirrors);
    return totReflectivity;
}

//Check if many vectors have same size
bool vectorSizes(std::vector<std::size_t> sizes)
{
    uint32_t size = sizes.at(0);
    uint32_t nVectors = sizes.size();

    for (uint32_t i; i < nVectors; i++)
    {
        if (sizes.at(i) != size)
        {
            return false;
        }
    }

    return true;
}

} // namespace astroUtilities