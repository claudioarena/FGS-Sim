/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run Monte Carlo simulation of centroid determination error with input star at a position determined according to a uniform distribution
 *
 * @file MonteCarlo.cpp
 * @brief Monte Carlo simulation of centroid determination error
 * @author Feiyu Fang
 * @version 2.2 26-08-2017
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "MonteCarlo.hpp"

using namespace std;

/**
 * Constructor to create a new MonteCarlo object with the input parameters. Opens a results file and outputs the parameters to the file.
 * @brief Constructs a MonteCarlo object with input parameters and opens file
 * 
 * @param fileName Name of output file; "results.csv" preferred. 
 * @param inX x-coordinate of star
 * @param inY y-coordinate of star
 * @param horizPixels Number of pixels in the x-axis to bin the data into
 * @param vertPixels Number of pixels in the y-axis to bin the data into
 * @param samp Number of data points ("simels") per pixel
 * @param t Integration time /s
 * @param diameter Diameter of telescope input pupil /m
 * @param qEff Quantum efficiency. Ideal 1.
 * @param temp Temperature of the sensor /K. Ideal -> 0.
 * @param e Emissivity of the dichroic. Ideal 1. 
 * @param readNoise Readout noise /electrons. Ideal 0. 
 * @param analogueDigitalUnits Analogue-to-digital units. Electrons per count; ideal 1.
 */
MonteCarlo::MonteCarlo(string fileName, float inX, float inY, int horizPixels, int vertPixels, int samp, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits) {
	xIn = inX;
	yIn = inY;
	xPixels = horizPixels;
	yPixels = vertPixels;
	sampling = samp;
	time = t;
	area = M_PI * pow((diameter / 2.), 2);
	QE = qEff;
	temperature = temp;
	emissivity = e;
	readout = readNoise;
	ADU = analogueDigitalUnits;
	
	xPoints = xPixels * sampling;
	yPoints = yPixels * sampling;

	// Open file and output parameters
	outFile.open(fileName);
	outFile << "Test: Varying sigma, Input centre: (" << xIn << ';' << yIn << "), Pixels in each dimension: ("
			<< xPixels << ';' << yPixels << "), Data points simulated in each dimension: (" << xPoints << "; "
			<< yPoints << ")," << "Exposure time: " << time << " s" << endl;
	outFile << "Telescope pupil area: " << area << " m^2, QE: " << QE << ", Temperature: " << temperature 
			<< " K, Emissivity of sensor: " << emissivity << ", Readout noise: " << readout << " electrons. " << endl;

	outFile << endl << "Sigma in both dimensions, Average distance, Photons in, Photons detected, Monte Carlo standard deviation" << endl;
}


/**
 * @brief Destructor for MonteCarlo object closes the current file
 */
MonteCarlo::~MonteCarlo() {outFile.close();}

/**
 * Private static function to return the total number of photons in a pixel grid, either of pixels or simels.
 *
 * @param matrix The pixel grid in a 2d int vector
 * @return Total number of photons in the inputted matrix
 */
int MonteCarlo::sumPhotons(vector<vector<int>> matrix) {
	
	int sum = 0;
	for (vector<int> v: matrix) {for (int i: v) sum += i;}
	return sum;
}

/**
 * Private static function to calculate averages
 * @brief Calculates the average of numbers held in a vector
 * @param in A vector<float> containing the integers whose average is to be found
 * @return Average number
 */
float MonteCarlo::average(vector<float> in) {

	float sum = accumulate(in.begin(), in.end(), 0.);
	return (sum / in.size());
}

/**
 * Private static function to calculate the standard deviation of a vector of floats
 * @brief Calculates the standard deviation of numbers held in a vector
 * @param in A vector<float> containing the numbers whose SD is to be found
 * @return Standard deviation
 */
float MonteCarlo::stdDev(vector<float> in) {
	
	float mean = average(in);
	float accum = 0.0;
	for_each (begin(in), end(in), [&](const float d) {accum += (d - mean) * (d - mean);});

	return sqrt(accum / (in.size()-1));
}

/**
 * Run Monte Carlo simulation for star at a random position within a pixel with a given magnitude, running for n times
 * 
 * @brief Run Monte Carlo simulation
 * @param mag Magnitude of the star to be tested
 * @param n Number of times to run simulation for each sigma
 */
void MonteCarlo::run(float mag, int iterations) {

	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	outFile << endl << "Magnitude: " << mag << endl;
	for (float i = 1; i <= 10; i += 1) { // Run test varying sigma for each magnitude
		
		cout << "Calculating for magnitude = " << mag << ", sigma = " << i << " ..." << endl;
		vector<float> errors; // Vector to hold the error from each Monte Carlo simulation
		vector<float> photonsIn; 
		vector<float> photonsOut;

		for (int j = 0; j < iterations; j++) { /// Iterate each sigma x times at random star positions and find average
			int N = pow(2.512, -1 * mag) * 3.36E10; // Convert magnitude to photons s^-1 m^-2. TODO: Use band-specific normalisation to introduce band magnitudes. 
			float uniformX = xIn + distribution(generator);
			float uniformY = yIn + distribution(generator);
			Test* t = new Test(N, uniformX * sampling, uniformY * sampling, i, i, xPixels, yPixels, xPoints, yPoints);
			t->run(true, time, area, QE, temperature, emissivity, readout, ADU); // Run with noise for input time and area 

			float x = (t->xCentre * xPixels);
			float y = (t->yCentre * yPixels);
			errors.push_back(sqrt((x - uniformX)*(x - uniformX) + (y - uniformY)*(y - uniformY)));
			photonsIn.push_back(sumPhotons(t->gaussianInput));
			photonsOut.push_back(sumPhotons(t->pixelData));
			delete t;
		}
		outFile << i << ',' << average(errors) << ',' << average(photonsIn) << ',' << average(photonsOut) << ',' << stdDev(errors) << endl;
	}
}
