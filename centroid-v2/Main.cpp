/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Generate a 2d Gaussian with noise, bin it into "pixels" and find the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.1 28-07-2017
 */

#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#include "Test.hpp"

using namespace std;

/**
 * @brief Test one run, outputting to the console instead of a file. 
 *
 * @param N Number of photons to detect
 * @param xIn x-coordinate of star
 * @param yIn y-coordinate of star
 * @param sigmaX Standard deviation of Gaussian in x-axis
 * @param sigmaY Standard deviation of Gaussian in y-axis
 * @param xPixels Number of pixels to bin the photons into in the x-axis
 * @param yPixels Number of pixels to bin the photons into in the y-axis
 * @param points Number of data points to simulate the input Gaussian with
 * @param time Integration time /s
 * @param area Area of telescope aperture /ms^-2
 */
void runOnce(int N, float xIn, float yIn, float sigmaX, float sigmaY, int xPixels, int yPixels, int points, float time, float area) {

	cout << "Total photons: " << N << endl;
	Test* t = new Test(N, xIn, yIn, sigmaX, sigmaY, xPixels, yPixels, points);
	t->run(true, time, area); // Run with noise for input time and area 
	float x = (t->xCentre * xPixels);
	float y = (t->yCentre * yPixels);
	float expectedX = xIn * xPixels / points;
	float expectedY = yIn * yPixels / points;
	cout << "The input coordinates were (" << expectedX << ',' << expectedY << "). The calculated centroid was (" << x << ',' << y << ")." << endl;
	cout << "Error in pixel units = " << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y - expectedY)) << endl << endl;
	Test::print2dVector(t->pixelData);
	
	int sum = 0;
	for (vector<int> v: t->pixelData) {
		for (int i: v) sum += i;
	}
	cout << "Sum of detected photons = " << sum << endl << endl;
	delete t;
}

/**
 * @brief Test multiple runs, outputting the results to a CSV file. 
 *
 * @param N Number of photons to detect
 * @param xIn x-coordinate of star
 * @param yIn y-coordinate of star
 * @param sigmaX Standard deviation of Gaussian in x-axis
 * @param sigmaY Standard deviation of Gaussian in y-axis
 * @param xPixels Number of pixels to bin the photons into in the x-axis
 * @param yPixels Number of pixels to bin the photons into in the y-axis
 * @param points Number of data points to simulate the input Gaussian with
 * @param time Integration time /s
 * @param area Area of telescope aperture /ms^-2
 */
void runToFile(int N, float xIn, float yIn, float sigmaX, float sigmaY, int xPixels, int yPixels, int points, float time, float area) {

	// For testing multiple runs, outputting to an output csv file
	ofstream outFile; // Initialise output file
	outFile.open("results.csv");
	outFile << "Input centre: (" << xIn << ';' << yIn << "), pixels in each dimension: " << xPixels << ';' 
			<< yPixels << ", data points simulated in each dimension: " << points << endl;

	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	outFile << "Running the simulation 10 times for a star position which varies between -0.5 to +0.5 from the input centre,," << endl;
	for (int i = 0; i < 10; i++) {

		Test* t = new Test(N, xIn + distribution(generator), yIn + distribution(generator), sigmaX, sigmaY, xPixels, yPixels, points);
		t->run(true, time, area); // Run with noise, with time and area
		float x = (t->xCentre * xPixels); // Convert output to pixels
		float y = (t->yCentre * yPixels);
		float expectedX = xIn * xPixels / points;
		float expectedY = yIn * yPixels / points;
		outFile << i << ',' << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y - expectedY)) << ',' << x << ',' << y << endl;
		delete t;
	}

	outFile << endl << "Varying sigma: " << endl;
	outFile << "Sigma in both dimensions, Distance, x-centre, y-centre" << endl;

	for (float i = 10; i <= 100; i+=10) { // Run test varying sigma

		Test* t = new Test(N, xIn, yIn, i, i, xPixels, yPixels, points);
		t->run(true, time, area); // Run with noise, with time and area
		float x = (t->xCentre * xPixels);
		float y = (t->yCentre * yPixels);
		float expectedX = xIn * xPixels / points;
		float expectedY = yIn * yPixels / points;
		outFile << i << ',' << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y - expectedY)) << ',' << x << ',' << y << endl;
		delete t;
	}
	outFile.close();
}

///Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	float magnitude = 10; // Star magnitude
	float xIn = 20; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 30;
	int xPixels = 10;
	int yPixels = xPixels;
	int sampling = 10; // Pixel sampling: Simulated points per pixel

	int points = sampling * xPixels; 
	float N = pow(2.512, -1 * magnitude) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
	
	runOnce(N, xIn, yIn, 10, 10, xPixels, yPixels, points, 1, 1);
	//runToFile(N, xIn, yIn, 10, 10, xPixels, yPixels, points, 1, 1);

	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl;

	return 0;
}

// TODO: DONE - Change Poisson noise (photon noise in space) into an array of poisson(sqrt(N)). 
// DONE - This depends on the area of the telescope and the integration time (star exposure time). 
// DONE - Each value in the Gaussian array should have calculated a corresponding value in a noise array. 
// DONE - The noise in the pixel is then the RMS of all of the noise array elements being binned into the pixel. 
// DONE - Check if this is the same as just the sqrt of the pixel binned value. 
// Look at photons per second for a given magnitude. 
// Have the separate parameters of an input star magnitude and the integration time, keeping one constant and varying the other. 
// 1/(sampling frequency) = (exposure time + readout time). 
//
// Have parameters for things like quantum efficiency and dark current, instrument emissivity = 1. 
// DONE - Use a uniform distribution inside a pixel to define the starting position between simulated Gaussian2d points. 
// Relate sigma and mu in PSF
// DONE - Fix memory leak. 
// DONE - Define the edges as going from 0 to 1 in both dimensions, and then manipulate afterwards. 
// DONE - Play around with 1x1 and 2x2, etc small inputs. 
