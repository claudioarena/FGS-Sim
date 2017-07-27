/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Class to generate a 2d vector Gaussian and add Gaussian noise. 
 *
 * @file Gauss2d.cpp
 * @brief Generate a 2d Gaussian with noise
 * @author Feiyu Fang 
 * @version 2.0.1 24-07-2017
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "Gauss2d.hpp"

using namespace std;

/**
 * Constructs a Gauss2d object to generate a 2d Gaussian
 *
 * @param points The number of data points in each dimension
 * @param cx X-coordinate for Gaussian centre
 * @param cy Y-coordinate for Gaussian centre
 * @param sdX Standard deviation of the Gaussian in the x direction
 * @param sdY Standard deviation of the Gaussian in the y direction
 */
Gauss2d::Gauss2d(int xPoints, int yPoints, float cx, float cy, float sdX, float sdY) {

	hX = xPoints;
	hY = yPoints;
	centreX = cx;
	centreY = cy;
	sigmaX = sdX;
	sigmaY = sdY;
}

Gauss2d::~Gauss2d() {}

// Calculate Gaussian value at x, with mean mu and SD sigma
float Gauss2d::gaussDist(float x, float mu, float sigma) {
	
	return (1 / sqrt(2 * M_PI * sigma*sigma)) * exp(-1 * pow(x - mu, 2) / (2 * sigma*sigma));
}

/**
 * Cumulative normal distribution function to integrate between inputted limits for a Gaussian with inputted 
 * mean and standard deviation. Uses erfc from <cmath> as F(x) is closely related to error function. 
 *
 * @brief Integrate a specified Gaussian between limits
 *
 * @param a Lower limit of integration
 * @param b Upper limit of integration
 * @param mean Mean of the Gaussian
 * @param sigma Standard deviation of the Gaussian
 *
 * @return Value of integrated area
 */
float Gauss2d::normalCDF(float a, float b, float mean, float sigma) {
	
	float z1 = (a - mean) / sigma; // Standardising the normal distribution to Z~N(0,1)
	float z2 = (b - mean) / sigma;
	return 0.5 * (erfc(-z2 * M_SQRT1_2) - erfc(-z1 * M_SQRT1_2));
}

/**
 * Static function to generate a vector<int> of a Gaussian of integers sorted into bins. 

 * @brief Generates a vector<int> of a Gaussian
 *
 * @param N Number of events to bin
 * @param bins Number of bins to sort the events into
 * @param mean Mean of the Gaussian
 * @param sigma Standard deviation of the Gaussian
 *
 * @return A vector<int> containing the events sorted into bins
 */
vector<int> Gauss2d::generateIntVector(int N, int bins, float mean, float sigma) {

	vector<int> out; // Vertical array of all the numbers of photons to be distributed per row

	for (int i = 0; i < bins; i++) {
		out.push_back(normalCDF(i, i+1, mean, sigma) * N);
	}
	return out;
}

// Returns a 2d Gaussian array centred at x, y, valued at 2h points with SD_x sx and SD_y sy. 
vector<vector<float>> Gauss2d::generate() {
	int w = 2 * hX; // Width of generated arrays
	vector<vector<float>> gauss2d;
	float iValue, jValue;

	// Generate Gaussians in two dimensions, and then multiply elements to create a 2d Gaussian array.
	for (int i = 0; i < w; i++) {
		vector<float> row;
		iValue = gaussDist((i + 0.5) / hX, centreY, sigmaX);
		
		for (int j = 0; j < w; j++) {
			jValue = gaussDist((j + 0.5) / hX, centreX, sigmaY); 
			row.push_back(iValue * jValue); 
		}

		gauss2d.push_back(row);
	}
	return gauss2d;
}
