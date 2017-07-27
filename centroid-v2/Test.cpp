/**
 *	Twinkle FGS-Sim: Centroid recovery simulation
 *	Purpose: Take a noisy 2d Gaussian and bin it into pixels, representing defocussing of a star. 
 *  Then calculate the centroid from the pixel data. 
 *
 *	@file Test.cpp
 *	@brief Bin an inputted Gaussian 2d array and find its centroid
 *	@author Feiyu Fang
 *	@version 2.0.1 25-07-2017
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
 * @param xIn X-coordinate of the Gaussian 2d vector to be generated
 * @param yIn Y-coordinate of the Gaussian 2d vector to be generated
 * @param sdX Standard deviation of the generated Gaussian 2d vector in the x-direction
 * @param sdY Standard deviation of the generated Gaussian 2d vector in the y-direction
 * @param hPixels Number of bins in the x-direction to sort the data into
 * @param vPixels Number of bins in the y-direction to sort the data into
 * @param points Number of data points per dimension to simulate
 */
Test::Test(float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int points) {
	inX = xIn;
	inY = yIn;
	sigmaX = sdX;
	sigmaY = sdY;
	horizPixels = hPixels;
	vertPixels = vPixels;
	h = points;
}

Test::~Test() {}

/**
 * Static method to sum an inputted 2d array vertically. 
 * Overloaded to both input and output either float or int. 
 *
 * @param in vector<vector<float>> input to be summed over
 * @param i Beginning line index to sum from
 * @param end Number of lines to sum over, starting from parameter i
 * @return A vector<float> containing the vertical sums
 */
vector<float> Test::sumVert(vector<vector<float>> in, int i, int end) {

	vector<float> yOut;
	int w = in.at(0).size(); // Width of input data

	for (int j = 0; j < w; j++) { // Calculate vertical subtotal over a single column
		float subTotal = 0;
		for (int k = i; k < (i + end); k++) subTotal += in.at(k).at(j);
		yOut.push_back(subTotal);
	}

	return yOut;
}
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
 * Take the current Test object and add Poisson noise to the pixelData. 
 *
 * @param time Exposure time
 * @param area Telescope aperture area
 * @return outNoise A 2d vector of just the generated noise
 */
vector<vector<int>> Test::addPoissonNoise(float time, float area) {

	// Seed the generation of Poisson-distributed random variable with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);

	vector<vector<int>> outData;
	vector<vector<int>> outNoise;
	
	for (vector<int> v: this->pixelData) { // For each row of the existing pixel data
		vector<int> rowOutData;
		vector<int> rowOutNoise;

		for (int i: v) { // For each pixel value in the row, generate its noise and add to output vector. 

			poisson_distribution<int> distribution(sqrt(i)); // Noise ~ Po(sqrt(value))
			int noiseAddition = distribution(generator) * time * area;
			rowOutNoise.push_back(noiseAddition);
			rowOutData.push_back(noiseAddition + i);
		}

		outData.push_back(rowOutData);
		outNoise.push_back(rowOutNoise);
	}

	this->pixelData = outData;
	return outNoise;
}

/**
 *	Static function to sort an inputted 2d vector into bins, representing the collection of photons in pixels.
 *
 *	@brief Sorts input data into bins.
 *	@param dataIn A 2d vector containing the raw Gaussian data
 *	@param h Number of horizontal pixels to bin the data into
 *	@param v Number of vertical pixels to bin the data into
 */
vector<vector<int>> Test::binData(vector<vector<float>> dataIn, int h, int v) {

	vector<vector<float>> outputFloat;
	vector<vector<int>> out;
	int inHeight = dataIn.size();
	int inWidth = dataIn.at(0).size();
	int pixelHeight = inHeight / v; // Height and width of output 2d vector: 
	int pixelWidth = inWidth / h;  // number of input elements per bin in each dimension
	
	vector<vector<float>> xBinned; // Vector where the data have been binned in x, but not yet in y. 
	for (vector<float> v: dataIn) { // Loop to bin data in x and store in xBinned
		float total = 0;
		vector<float> xTotals;
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
		outputFloat.push_back(Test::sumVert(xBinned, i, pixelHeight)); // Put a row of pixels into the output vector<vector<float>>
		i += pixelHeight;
	}
	
	// Cast all the values in the 2d vector from float into int, as photons are quantised. Output to pixelData.
	for (vector<float> v: outputFloat) {
		vector<int> rowOut;
		for (float f: v) rowOut.push_back((int)f);
		out.push_back(rowOut);
	}
	return out;
}

/**
 * Method to find the centroid of the pixel data and put its co-ordinates into this object
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
 * Prints a 2d vector to the console
 * @param data Data to be printed
 * @param RMS Whether to print the sqrt of all the elements
 */
void Test::print2dVector(vector<vector<int>> data, bool RMS) {

	for (int i = 0; i < (int)data.size(); i++) { // Loop through each row
		if (RMS == true) for (int j = 0; j < (int)data.at(0).size(); j++) cout << setw(9) << sqrt(data.at(i).at(j));
		else for (int j = 0; j < (int)data.at(0).size(); j++) cout << setw(9) << data.at(i).at(j);
		cout << endl;
	}
	cout << endl;
}

/**
 * Static method to run the centroid determination simulation and return a Test object containing the results. 
 *
 * @brief Run centroid determination simulation for input variables
 * @param noise Boolean whether to add Poisson noise to the pixel data
 * @param time Integration time, as more time means more photons collected
 * @param area Area of telescope aperture, as more area means more photons
 */
void Test::run(bool noise, float time, float area) {

	// Generate a h x h 2D array of a Gaussian with noise. 
	Gauss2d *g = new Gauss2d(h/2, h/2, inX, inY, sigmaX, sigmaY);
	gaussianInput = g->generate();

	pixelData = binData(gaussianInput, horizPixels, vertPixels);
	if (noise == true) {
		noiseAfterBin = this->addPoissonNoise(time, area);
		//print2dVector(noiseAfterBin, false); // For debug only
	}
	this->findCentroid();

	delete g;
}
