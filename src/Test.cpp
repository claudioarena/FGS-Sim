/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Take a 2d PSF, add noise and bin it into pixels, representing the PSF of a star. 
 * Then calculate the centroid from the pixel data. 
 *
 * @file Test.cpp
 * @brief Bin an inputted PSF 2d array and find its centroid
 * @author Feiyu Fang
 * @version 3.2.1 2018-03-21
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "Test.hpp"
#include "astroUtilities.hpp"

using namespace std;

/**
 * Constructs a Test object to bin inputted data and find its centroid. 
 *
 * @param nPhotons Number of photons to simulate
 * @param inX Input star centre x-coordinate
 * @param inY Input star centre y-coordinate
 * @param hPixels Number of bins in the x-direction to sort the data into
 * @param vPixels Number of bins in the y-direction to sort the data into
 * @param zodiac Whether to include zodiacal light as a background
 * @param name File name of the PSF data file to be imported
 */

//PSF Constructor
Test::Test(int nPhotons, float inX, float inY, int hPixels, int vPixels, bool zodiac, string name)
{

	N = nPhotons;
	xIn = inX;
	yIn = inY;
	horizPixels = hPixels;
	vertPixels = vPixels;
	zodiacal = zodiac;
	filename = name;
}

/* // Gaussian Constructor
Test::Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int xPoints, int yPoints, bool zodiac)
{

	N = nPhotons;
	xIn = xIn;
	yIn = yIn;
	sigmaX = sdX;
	sigmaY = sdY;
	horizPixels = hPixels;
	vertPixels = vPixels;
	pointsX = xPoints;
	pointsY = yPoints;
}*/

Test::~Test() {}

/**
 * Private function to take the current Test object and add Poisson noise, dark noise, readout noise, mirror 
 * thermal emission and zodiacal light to the pixelData. Currently returns a 2d vector of just the generated noise for 
 * debugging purposes, but this function can be changed to private void if necessary. 
 *
 * @param time Exposure time /s
 * @param area Telescope aperture area /ms^-2
 * @param QE Quantum efficiency of the sensor. Ideal 1. 
 * @param temperature Temperature of the sensor. Ideal -> 0.
 * @param emissivity Emissivity of the sensor. Ideal blackbody 1. 
 * @param readout Readout noise /electrons, quoted as variance. Ideal 0.
 * @param ADU Analogue-to-digital units. Electrons per count; ideal 1. 
 * @param darkSignal Dark current /electrons. 
 * @return outNoise A 2d vector of just the generated noise. 
 */
vector<vector<int>> Test::addNoise(float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal)
{
}