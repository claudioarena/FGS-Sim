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
Gauss2d::Gauss2d(int points, float cx, float cy, float sdX, float sdY) {

	h = points;
	centreX = cx;
	centreY = cy;
	sigmaX = sdX;
	sigmaY = sdY;
}

Gauss2d::~Gauss2d() {}

// Calculate Gaussian value at x, with mean mu and SD sigma
float Gauss2d::gaussDist(float x, float mu, float sigma) {return (1 / sqrt(2 * M_PI * sigma*sigma)) * exp(-1 * pow(x - mu, 2) / (2 * sigma*sigma));}

// Returns a 2d Gaussian array centred at x, y, valued at 2h points with SD_x sx and SD_y sy. 
vector<vector<float>> Gauss2d::generate() {
	int w = 2 * h; // Width of generated arrays
	vector<vector<float>> gauss2d;
	float iValue, jValue;

	// Generate Gaussians in two dimensions, and then multiply elements to create a 2d Gaussian array.
	for (int i = 0; i < w; i++) {
		vector<float> row;
		iValue = gaussDist((i + 0.5) / h, centreY, sigmaX);
		
		for (int j = 0; j < w; j++) {
			jValue = gaussDist((j + 0.5) / h, centreX, sigmaY); 
			row.push_back(iValue * jValue); 
		}

		gauss2d.push_back(row);
	}
	return gauss2d;
}
