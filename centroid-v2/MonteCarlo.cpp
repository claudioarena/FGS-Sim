/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run Monte Carlo simulation of centroid determination error with input star at a position determined according to a uniform distribution
 *
 * @file MonteCarlo.cpp
 * @brief Monte Carlo simulation of centroid determination error
 * @author Feiyu Fang
 * @version 2.2 26-08-2017
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "MonteCarlo.hpp"

using namespace std;

/**
 * @brief Constructs a MonteCarlo object with input parameters
 *
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
MonteCarlo::MonteCarlo(float inX, float inY, int horizPixels, int vertPixels, int samp, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits) {
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
}

MonteCarlo::~MonteCarlo() {};

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
 * Public method to run the Monte Carlo test. 
 * @brief Test multiple runs, outputting the results to a CSV file. 
 * @param fileName Name of output file. Recommended "results.csv". 
 */
void MonteCarlo::runToFile(string fileName) {

	int xPoints = xPixels * sampling;
	int yPoints = yPixels * sampling;
	
	// For testing multiple runs, outputting to an output csv file
	ofstream outFile; // Initialise output file
	outFile.open(fileName);

	outFile << "Input centre: (" << xIn << ';' << yIn << "); pixels in each dimension: (" << xPixels << ';' 
			<< yPixels << "); data points simulated in each dimension: (" << xPoints << "; " << yPoints << ')' << endl;
	outFile << "Exposure time: " << time << " s; Telescope pupil area: " << area << " m^2; QE: " << QE
			<< "; Temperature: " << temperature << " K; Emissivity of sensor: " << emissivity << "; Readout noise: "
			<< readout << " electrons. " << endl;
	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	outFile << endl << "Varying sigma: " << endl;
	outFile << "Sigma in both dimensions, Distance, x-centre, y-centre, photons in, photons detected" << endl;

	for (int mag = 7; mag <= 13; mag += 3) { // Run test varying magnitude
		outFile << endl << "Magnitude: " << mag << endl;
		for (float i = 1; i <= 10; i += 1) { // Run test varying sigma for each magnitude
			int N = pow(2.512, -1 * mag) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
			float uniformX = xIn + distribution(generator);
			float uniformY = yIn + distribution(generator);
			Test* t = new Test(N, uniformX * sampling, uniformY * sampling, i, i, xPixels, yPixels, xPoints, yPoints);
			t->run(true, time, area, QE, temperature, emissivity, readout, ADU); // Run with noise for input time and area 

			float x = (t->xCentre * xPixels);
			float y = (t->yCentre * yPixels);
			outFile << i << ',' << sqrt((x - uniformX)*(x - uniformX) + (y - uniformY)*(y - uniformY)) << ','
					<< x << ',' << y << ',' << sumPhotons(t->gaussianInput) << ',' << sumPhotons(t->pixelData) << endl;
			delete t;
		}
	}
	outFile.close();
}
