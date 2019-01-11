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
 * Private static method to sum an inputted 2d array vertically. 
 *
 * @param in vector<vector<int>> input to be summed over
 * @param i Beginning line index to sum from
 * @param end Number of lines to sum over, starting from parameter i
 * @return A vector<int> containing the vertical sums
 */
vector<int> Test::sumVert(vector<vector<int>> in, int i, int end)
{

	vector<int> yOut;
	int w = in.at(0).size(); // Width of input data

	for (int j = 0; j < w; j++)
	{ // Calculate vertical subtotal over a single column
		int subTotal = 0;
		for (int k = i; k < (i + end); k++)
			subTotal += in.at(k).at(j);
		yOut.push_back(subTotal);
	}

	return yOut;
}

/**
 * Public static function to find the number of photons per second for a given band-magnitude. Defaults to 
 * combination of B, V and R bands. 
 *
 * @brief Finds the number of photons per second for a given band and magnitude. 
 * @param mag Band-magnitude of the star
 * @param band Spectral band: B, V, R. 
 * @return Number of photons from a star in the given band per second
 */

double Test::photonsInAllBands(std::vector<float> mags, std::vector<filter> fltrs)
{
	if (sizeof(mags) != sizeof(fltrs))
	{
		throw("error. Number of specified magnitudes and filters must match");
	}
	if (sizeof(mags) < 1)
	{
		throw("error. At least one magnitude must be specified");
	}

	double photons = 0;
	for (int i = 0; i < sizeof(mags); i++)
	{
		photons += Test::photonsInBand(mags[i], fltrs[i]);
	}

	return photons;
}

double Test::photonsInBand(float mag, struct filter flt)
{
	double m0_photons = (1.51 * (10 ^ 7)) * (flt.zero_point_Jy * (flt.bandWidth / flt.centerBand));

	double photons = pow(2.512, -1 * mag) * m0_photons; // Photons per second, per m-2
	return photons;
}

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
	float power = area * emiss * SB_CONST * pow(temperature, 4);
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
		zodiacalPhotons += (180. / this->horizPixels) * (180. / this->vertPixels) * (photonsInBand(22.37, B_filter) + photonsInBand(21.89, V_filter)); // Background photons from zodiacal light. See line 84 of ../README.md.
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

/**
 *	Private function to sort an inputted 2d vector into bins, representing the collection of photons in pixels.
 *
 *	@brief Sorts input data into bins.
 *
 *	@param dataIn A 2d vector containing the raw Gaussian data
 *	@param h Number of horizontal pixels to bin the data into
 *	@param v Number of vertical pixels to bin the data into
 */
void Test::binData(vector<vector<int>> dataIn, int h, int v)
{

	int inHeight = dataIn.size();
	int inWidth = dataIn.at(0).size();
	int pixelHeight = inHeight / v; // Height and width of output 2d vector:
	int pixelWidth = inWidth / h;   // number of input elements per bin in each dimension

	vector<vector<int>> xBinned; // Vector where the data have been binned in x, but not yet in y.
	for (vector<int> v : dataIn)
	{ // Loop to bin data in x and store in xBinned
		int total = 0;
		vector<int> xTotals;
		int i = 0;

		while (i < inWidth)
		{ // Bin each row of data
			for (int j = 0; j < pixelWidth; j++)
			{
				total += v.at(i + j);
			}
			xTotals.push_back(total);
			total = 0;
			i += pixelWidth;
		}
		xBinned.push_back(xTotals);
	}

	// At this point, the data has been binned horizontally, but not yet vertically.

	// Bin data vertically
	int i = 0;
	while (i < inHeight)
	{																 // Loop over each pixel
		pixelData.push_back(Test::sumVert(xBinned, i, pixelHeight)); // Put a row of pixels into the output vector<vector<float>>
		i += pixelHeight;
	}
}

/**
 * Private method to find the centroid of the pixel data and put its co-ordinates into this object. Calculates
 * centroid by summing the pixel matrix in each dimension and calculating a weighted mean for both dimension.
 */
void Test::findCentroid()
{

	float totalWeight = 0; // Sum over all elements of the 2d array to find the weighting
	for (vector<int> v : pixelData)
	{
		for (float f : v)
			totalWeight += f;
	}

	int w = pixelData.at(0).size(); // Width of inputted array
	int h = pixelData.size();

	// Calculate horizontal centre
	vector<int> horizontalSum = Test::sumVert(pixelData, 0, h); // Sum the binned data vertically
	float sum = 0;
	for (int pixelIndex = 0; pixelIndex < w; pixelIndex++)
	{ // Calculate weighted mean of vertically summed data
		sum += (horizontalSum.at(pixelIndex) * (pixelIndex + 0.5));
	}
	xCentre = (sum / totalWeight) / w;

	// Calculate vertical centre
	sum = 0;
	int rowIndex = 0;
	for (vector<int> v : pixelData)
	{ // Each row of data contains a vector<int>...
		int rowSum = 0;
		for (int f : v)
			rowSum += f; // ...sum over these vector<int> and calculate the weighted mean.
		sum += (rowSum * (rowIndex + 0.5));
		rowIndex++;
	}
	yCentre = (sum / totalWeight) / h;
}

/**
 * Public static function to print a 2d vector to the console for debugging.
 * @param data Data to be printed
 */
void Test::print2dVector(vector<vector<int>> data)
{

	for (int i = 0; i < (int)data.size(); i++)
	{ // Loop through each row
		for (int j = 0; j < (int)data.at(0).size(); j++)
			cout << setw(9) << data.at(i).at(j);
		cout << endl;
	}
	cout << endl;
}

/**
 * Public static function to add together two matrices element-wise. 
 *
 * @brief Add two matrices
 * @param a First matrix to be added
 * @param b Second matrix to be added
 * @return Output matrix
 */
vector<vector<int>> Test::addMatrices(vector<vector<int>> a, vector<vector<int>> b)
{

	vector<vector<int>> out;
	vector<int> rowOut = a.at(0); // Output row needs to be initialised to the right dimensions for std::transform to work
	for (unsigned i = 0; i < a.size(); i++)
	{
		transform(a.at(i).begin(), a.at(i).end(), b.at(i).begin(), rowOut.begin(), std::plus<int>()); // Add each row element-wise
		out.push_back(rowOut);
	}
	return out;
}

/**
 * Public method to run the centroid determination simulation and put the results into the member variables of this class. 
 *
 * @brief Run centroid determination simulation for input variables
 * 
 * @param noise Boolean whether to add noise and non-ideal conditions to the pixel data
 * @param huygens Whether the PSF is Huygens or FFT. True for Huygens, false for FFT. 
 * @param time Integration time, as more time means more photons collected
 * @param area Area of telescope aperture, as more area means more photons
 * @param QE Quantum efficiency of the CCD. Ideal 1
 * @param temperature Temperature of the sensor /K. Ideal 0. 
 * @param emissivity Emissivity of the sensor. Ideal blackbody 1.
 * @param readout Readout noise /electrons. Ideal 0. 
 * @param ADU Analogue-to-digital units. Electrons per count; ideal 1. 
 * @param darkSignal Dark signal /electrons
 * @param motion Pointer to Brownian object containing the parameters for generating Brownian motion
 * @param brownianRuns Number of Brownian motion updates to do within a single camera exposure. TODO: Change this to link to sampling frequency, or something
 */
void Test::run(bool noise, bool huygens, float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal, Brownian *motion, int brownianRuns)
{

	int nPhotons = N * time * area / brownianRuns;
	PSF *p = new PSF(filename, nPhotons, huygens); // Generate a new matrix of photon data from the inputted PSF

	// Initialise the vector<vector<int>> first, then add the simel matrices for a star moving within an exposure
	for (int i = 0; i < brownianRuns; i++)
	{
		motion->generate();
		if (i == 0)
			simelsIn = p->samplePhotons(xIn + motion->brownianDx, yIn + motion->brownianDy);
		else
			simelsIn = addMatrices(simelsIn, p->samplePhotons(xIn + motion->brownianDx, yIn + motion->brownianDy));
	}

	this->binData(simelsIn, horizPixels, vertPixels);
	if (noise == true)
		noiseAfterBin = this->addNoise(time, area, QE, temperature, emissivity, readout, ADU, darkSignal);
	this->findCentroid();
	//if (huygens == true) print2dVector(pixelData);
	cout << "Photons detected: " << PSF::sum(pixelData) << ", total photons " << nPhotons << '.' << endl;

	delete p;
}
