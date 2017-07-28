/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Generate a 2d Gaussian with noise, bin it into "pixels" and find the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.0.1 25-07-2017
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

//Main method. Creates a 2d Gaussian, then bins it into pixels, then finds centroid.
int main() {
	
	time_t startTime = time(nullptr);
	cout << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	float xIn = 20; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 30;
	int xPixels = 10;
	int yPixels = xPixels;
	int sampling = 10; // Pixel sampling: Simulated points per pixel

	int points = sampling * xPixels; 
	
//	// For testing just one run, outputting to the console instead of a file. 
//	Test* t = new Test(1E6, xIn, yIn, 10, 10, xPixels, yPixels, points);
//	t->run(true, 1, 1); // Run with noise for time 1 and area 1
//	float x = (t->xCentre * xPixels);
//	float y = (t->yCentre * yPixels);
//	float expectedX = xIn / sampling;
//	float expectedY = yIn / sampling;
//	cout << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y -	expectedY)) << ',' << x << ',' << y << endl;
//	Test::print2dVector(t->pixelData);
//	delete t;

	// For testing multiple runs, outputting to an output csv file
	ofstream outFile; // Initialise output file
	outFile.open("results.csv");
	outFile << "Input centre: (" << xIn << ',' << yIn << "); pixels in each dimension: " << xPixels << ',' 
			<< yPixels << "; data points simulated in each dimension: " << points << endl;

	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);
	xIn += distribution(generator);
	yIn += distribution(generator);

	outFile << "Running the simulation 10 times for a star position which varies between -0.5 to +0.5 from the input centre" << endl;
	for (int i = 0; i < 10; i++) {

		Test* t = new Test(1E6, xIn, yIn, 10, 10, xPixels, yPixels, points);
		t->run(true, 1, 1); // Run with noise, with time 1 and area 1
		float x = (t->xCentre * xPixels); // Convert output to pixels
		float y = (t->yCentre * yPixels);
		float expectedX = xIn / sampling;
		float expectedY = yIn / sampling;
		outFile << i << ',' << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y - expectedY)) << ',' << x << ',' << y << endl;
		delete t;
	}

	outFile << endl << "Varying sigma: " << endl;
	outFile << "Sigma in both dimensions, Distance, x-centre, y-centre" << endl;

	for (float i = 10; i <= 100; i+=10) { // Run test varying sigma

		Test* t = new Test(1E6, xIn, yIn, i, i, xPixels, yPixels, points);
		t->run(true, 1, 1); // Run with noise, with time 1 and area 1
		float x = (t->xCentre * xPixels);
		float y = (t->yCentre * yPixels);
		float expectedX = xIn / sampling;
		float expectedY = yIn / sampling;
		outFile << i << ',' << sqrt((x - expectedX)*(x - expectedX) + (y - expectedY)*(y - expectedY)) << ',' << x << ',' << y << endl;
		delete t;
	}

	outFile.close();

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
