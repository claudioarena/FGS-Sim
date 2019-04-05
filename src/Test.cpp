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
 * Private static function to calculate the number of infrared photons emitted by the mirrors through thermal emission. 
 *
 * @brief Calculates noise from mirror infrared emission
 * @param area Area of mirror
 * @param temperature Temperature of mirror
 * @return Number of infrared photons emitted from mirror per second
 */
int Test::mirrorThermalNoise(float area, float temperature)
{
	float power = area * Twinkle.emiss * SB_CONST * pow(temperature, 4);
	float wavelength = WIEN / temperature;
	return power / (PLANCK * 3E8 / wavelength);
}

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

	// Seed the generation of Poisson-distributed random variable with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);

	int zodiacalPhotons = 0;
	if (zodiacal == true)
		zodiacalPhotons += (180. / this->horizPixels) * (180. / this->vertPixels) * (astroUtilities::photonsInBand(22.37, B_filter) + astroUtilities::photonsInBand(21.89, V_filter)); // Background photons from zodiacal light. See line 84 of ../README.md.
	//float dark = darkSignal * exp(5E-7 * 6.63E-34 * ((1./248)-(1./temperature)) / 1.38E-23); // Dark current variance for 500nm from Meyer-Neidel
	float dark = darkSignal * 122 * pow(temperature, 3) * exp(-6400. / temperature); // Dark current variation with temperature from E2V CCD230-42 datasheet
	poisson_distribution<int> darkCurrentDist(dark);								 // Noise from dark current
	poisson_distribution<int> readNoiseDist(readout);								 // Read noise
	poisson_distribution<int> mirrorThermalEmission(mirrorThermalNoise(area, temperature));
	poisson_distribution<int> zodiacalLight(zodiacalPhotons);
	vector<vector<int>> outData;
	vector<vector<int>> outNoise;

	for (vector<int> v : this->pixelData)
	{ // For each row of the existing pixel data
		vector<int> rowOutData;
		vector<int> rowOutNoise;

		for (int i : v)
		{ // For each pixel value in the row, generate its noise and add to output vector.
			float lambda = sqrt(i);
			poisson_distribution<int> photonNoise(lambda); // Noise ~ Po(sqrt(value))
			int noiseAddition = (lambda - photonNoise(generator)) + zodiacalLight(generator) + readNoiseDist(generator) + ((darkCurrentDist(generator) + (mirrorThermalEmission(generator) * area)) * time);
			rowOutNoise.push_back(noiseAddition);
			rowOutData.push_back((noiseAddition + i) * emissivity * QE * ADU);
		}

		outData.push_back(rowOutData);
		outNoise.push_back(rowOutNoise);
	}

	this->pixelData = outData;
	return outNoise;
}