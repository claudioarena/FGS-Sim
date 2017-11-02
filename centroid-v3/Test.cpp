/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Take a noisy 2d Gaussian and bin it into pixels, representing defocussing of a star. 
 * Then calculate the centroid from the pixel data. 
 *
 * @file Test.cpp
 * @brief Bin an inputted Gaussian 2d array and find its centroid
 * @author Feiyu Fang
 * @version 2.2.1 07-09-2017
 */

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
 * @param xIn X-coordinate of the Gaussian 2d vector to be generated
 * @param yIn Y-coordinate of the Gaussian 2d vector to be generated
 * @param sdX Standard deviation of the generated Gaussian 2d vector in the x-direction
 * @param sdY Standard deviation of the generated Gaussian 2d vector in the y-direction
 * @param hPixels Number of bins in the x-direction to sort the data into
 * @param vPixels Number of bins in the y-direction to sort the data into
 * @param xPoints Number of data points to simulate in the x-direction
 * @param yPoints Number of data points to simulate in the y-direction
 * @param zodiac Whether to include zodiacal light as a background
 */
Test::Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int xPoints, int yPoints, bool zodiac) {
	
	N = nPhotons;
	inX = xIn;
	inY = yIn;
	sigmaX = sdX;
	sigmaY = sdY;
	horizPixels = hPixels;
	vertPixels = vPixels;
	pointsX = xPoints;
	pointsY = yPoints;
	zodiacal = zodiac;
}

Test::~Test() {}

/**
 * Private static method to sum an inputted 2d array vertically. 
 *
 * @param in vector<vector<int>> input to be summed over
 * @param i Beginning line index to sum from
 * @param end Number of lines to sum over, starting from parameter i
 * @return A vector<int> containing the vertical sums
 */
vector<int> Test::sumVert(vector<vector<int>> in, int i, int end) {

	vector<int> yOut;
	int w = in.at(0).size(); // Width of input data

	for (int j = 0; j < w; j++) { // Calculate vertical subtotal over a single column
		int subTotal = 0;
		for (int k = i; k < (i + end); k++) subTotal += in.at(k).at(j);
		yOut.push_back(subTotal);
	}

	return yOut;
}

/**
 * Public static function to find the number of photons per second for a given band-magnitude. Defaults to 
 * combination of B, V and R bands. 
 * @brief Finds the number of photons per second for a given band and magnitude. 
 * @param mag Band-magnitude of the star
 * @param band Spectral band: B, V, R. 
 * @return Number of photons from a star in the given band per second
 */
int Test::photonsInBand(float mag, char band) {

	double out = pow(2.512, -1 * mag); // Photons per second = pow(2.512, -1 * mag) * 3.36E10, etc. 
	switch (band) {
		case 'B':
			out *= 1.415E10;
			break;
		case 'V':
			out *= 8.79E9;
			break;
		case 'R':
			out *= 1.07E10;
			break;
		default:
			out *= 3.36E10;
			break;
	}
	return out;
}

/**
 * Private static function to calculate the number of infrared photons emitted by the mirrors through thermal emission. 
 * @brief Calculates noise from mirror infrared emission
 * @param area Area of mirror
 * @param temperature Temperature of mirror
 * @return Number of infrared photons emitted from mirror per second
 */
int Test::mirrorThermalNoise(float area, float temperature) {
	const float SB = 5.67E-8;		// Stefan-Boltzmann constant
	const float WIEN = 2.9E-3;		// Wien's displacement constant
	const float PLANCK = 6.63E-34;	// Planck's constant
	float emiss = 0.02;				// Mirror emissivity

	float power = area * emiss * SB * pow(temperature, 4);
	float wavelength = WIEN / temperature;

	return power / (PLANCK * 3E8 / wavelength);
}

/**
 * Private function to take the current Test object and add Poisson noise to the pixelData. 
 *
 * @param time Exposure time /s
 * @param area Telescope aperture area /ms^-2
 * @param QE Quantum efficiency of the sensor. Ideal 1. 
 * @param temperature Temperature of the sensor. Ideal -> 0.
 * @param emissivity Emissivity of the sensor. Ideal blackbody 1. 
 * @param readout Readout noise /electrons, quoted as variance. Ideal 0.
 * @return outNoise A 2d vector of just the generated noise
 * @param ADU Analogue-to-digital units. Electrons per count; ideal 1. 
 * @param darkSignal Dark current /electrons. 
 * @param zodiacal Boolean specifying whether to add background photons from zodiacal light
 */
vector<vector<int>> Test::addNoise(float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal) {

	// Seed the generation of Poisson-distributed random variable with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);

	int zodiacalPhotons = 0;
	if (zodiacal == true) zodiacalPhotons += (180. / this->horizPixels) * (180. / this->vertPixels) * (photonsInBand(22.37, 'B') + photonsInBand(21.89, 'V')); // Background photons from zodiacal light. See line 84 of ../README.md. 
	float dark = darkSignal * exp(5E-7 * 6.63E-34 * ((1./248)-(1./temperature)) / 1.38E-23); // Dark current variance for 500nm

	poisson_distribution<int> darkCurrentDist(dark); // Noise from dark current
	poisson_distribution<int> readNoiseDist(readout); // Read noise
	poisson_distribution<int> mirrorThermalEmission(mirrorThermalNoise(area, temperature));
	poisson_distribution<int> zodiacalLight(zodiacalPhotons);
	vector<vector<int>> outData;
	vector<vector<int>> outNoise;
	
	for (vector<int> v: this->pixelData) { // For each row of the existing pixel data
		vector<int> rowOutData;
		vector<int> rowOutNoise;

		for (int i: v) { // For each pixel value in the row, generate its noise and add to output vector. 
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
void Test::binData(vector<vector<int>> dataIn, int h, int v) {

	int inHeight = dataIn.size();
	int inWidth = dataIn.at(0).size();
	int pixelHeight = inHeight / v; // Height and width of output 2d vector: 
	int pixelWidth = inWidth / h;  // number of input elements per bin in each dimension
	
	vector<vector<int>> xBinned; // Vector where the data have been binned in x, but not yet in y. 
	for (vector<int> v: dataIn) { // Loop to bin data in x and store in xBinned
		int total = 0;
		vector<int> xTotals;
		int i = 0;

		while (i < inWidth) { // Bin each row of data
			for (int j = 0; j < pixelWidth; j++) total += v.at(i+j);
			xTotals.push_back(total);
			total = 0;
			i += pixelWidth;
		}
		
		xBinned.push_back(xTotals);
	}
	
	// At this point, the data has been binned horizontally, but not yet vertically. 
	
	// Bin data vertically
	int i = 0;
	while (i < inHeight) { // Loop over each pixel
		pixelData.push_back(Test::sumVert(xBinned, i, pixelHeight)); // Put a row of pixels into the output vector<vector<float>>
		i += pixelHeight;
	}
}

/**
 * Private method to find the centroid of the pixel data and put its co-ordinates into this object
 */
void Test::findCentroid() { 

	float totalWeight = 0; // Sum over all elements of the 2d array to find the weighting
	for (vector<int> v: pixelData) {
		for (float f: v) totalWeight += f;
	}

	int w = pixelData.at(0).size(); // Width of inputted array
	int h = pixelData.size();
	
	// Calculate horizontal centre
	vector<int> horizontalSum = Test::sumVert(pixelData, 0, h); // Sum the binned data vertically
	float sum = 0; 
	for (int pixelIndex = 0; pixelIndex < w; pixelIndex++) { // Calculate weighted mean of vertically summed data
		sum += (horizontalSum.at(pixelIndex) * (pixelIndex + 0.5));
	}
	xCentre = (sum / totalWeight) / w;

	// Calculate vertical centre
	sum = 0;
	int rowIndex = 0;
	for (vector<int> v: pixelData) { // Each row of data contains a vector<int>...
		int rowSum = 0;
		for (int f: v) rowSum += f; // ...sum over these vector<int> and calculate the weighted mean.
		sum += (rowSum * (rowIndex + 0.5));
		rowIndex++;
	}
	yCentre = (sum / totalWeight) / h;
}

/**
 * Public static function to print a 2d vector to the console
 * @param data Data to be printed
 */
void Test::print2dVector(vector<vector<int>> data) {

	for (int i = 0; i < (int)data.size(); i++) { // Loop through each row
		for (int j = 0; j < (int)data.at(0).size(); j++) cout << setw(9) << data.at(i).at(j);
		cout << endl;
	}
	cout << endl;
}

/**
 * Public method to run the centroid determination simulation and return a Test object containing the results. 
 *
 * @brief Run centroid determination simulation for input variables
 * 
 * @param noise Boolean whether to add Poisson noise to the pixel data
 * @param time Integration time, as more time means more photons collected
 * @param area Area of telescope aperture, as more area means more photons
 * @param QE Quantum efficiency of the CCD. Ideal 1
 * @param temperature Temperature of the sensor /K. Ideal 0. 
 * @param emissivity Emissivity of the sensor. Ideal blackbody 1.
 * @param readout Readout noise /electrons. Ideal 0. 
 * @param ADU Analogue-to-digital units. Electrons per count; ideal 1. 
 * @param darkSignal Dark signal /electrons
 */
void Test::run(bool noise, float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal) {

	// Generate a 2D array of a Gaussian with noise. 
	Gauss2d *g = new Gauss2d(N * time * area, pointsX, pointsY, inX, inY, sigmaX, sigmaY);
	gaussianInput = g->generate();
	this->binData(gaussianInput, horizPixels, vertPixels);
	if (noise == true) noiseAfterBin = this->addNoise(time, area, QE, temperature, emissivity, readout, ADU, darkSignal);
	this->findCentroid();

	delete g;
}
