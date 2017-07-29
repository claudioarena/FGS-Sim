/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Class to generate a 2d vector Gaussian and add Gaussian noise. 
 *
 * @file Gauss2d.cpp
 * @brief Generate a 2d Gaussian with noise
 * @author Feiyu Fang 
 * @version 2.1 28-07-2017
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "Gauss2d.hpp"

using namespace std;

/**
 * Constructs a Gauss2d object to generate a 2d Gaussian
 *
 * @param nPhotons Number of photons in the 2d Gaussian
 * @param xPoints The number of data points in the x-axis
 * @param yPoints The number of data points in the y-axis
 * @param cx X-coordinate for Gaussian centre
 * @param cy Y-coordinate for Gaussian centre
 * @param sdX Standard deviation of the Gaussian in the x direction
 * @param sdY Standard deviation of the Gaussian in the y direction
 */
Gauss2d::Gauss2d(int nPhotons, int xPoints, int yPoints, float cx, float cy, float sdX, float sdY) {

	N = nPhotons;
	hX = xPoints;
	hY = yPoints;
	centreX = cx;
	centreY = cy;
	sigmaX = sdX;
	sigmaY = sdY;
}

Gauss2d::~Gauss2d() {}

/**
 * Private static function for cumulative normal distribution function to integrate between inputted limits for a Gaussian with inputted 
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
 * Public static function to generate a vector<int> of a Gaussian of integers sorted into bins. 

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

/**
 * Public function to generate a 2d Gaussian in a 2d array of integers, representing N photons being defocussed from a point
 * source and spread out into a Gaussian. 
 *
 * @brief Generates a 2d vector of N photons binned as a Gaussian. 
 *
 * @return The output 2d vector
 */
vector<vector<int>> Gauss2d::generateIntGaussian() {

	vector<vector<int>> out;
	vector<int> horizontalSummed = generateIntVector(N, hY, centreY, sigmaY);

	for (int i: horizontalSummed) {
		out.push_back(generateIntVector(i, hX, centreX, sigmaX));
	}
	return out;
}

