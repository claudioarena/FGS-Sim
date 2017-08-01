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
 * @param sampling Number of data points per pixel
 * @param time Integration time /s
 * @param area Area of telescope aperture /ms^-2
 * @param QE Quantum efficiency. Ideal 1.
 * @param temperature Temperature of the sensor /K. Ideal -> 0.
 * @param emissivity Emissivity of the sensor. Ideal 1.
 * @param readout Readout noise /electrons. Ideal 0.
 */
void runOnce(int N, float xIn, float yIn, float sigmaX, float sigmaY, int xPixels, int yPixels, int sampling, 
			 float time, float area, float QE, float temperature, float emissivity, int readout) {

	cout << "Total photons: " << N << endl;
	int xPoints = xPixels * sampling;
	int yPoints = yPixels * sampling;
	Test* t = new Test(N, xIn * sampling, yIn * sampling, sigmaX, sigmaY, xPixels, yPixels, xPoints, yPoints);
	t->run(true, time, area, QE, temperature, emissivity, readout); // Run with noise for input time and area 
	float x = (t->xCentre * xPixels);
	float y = (t->yCentre * yPixels);
	cout << "The input coordinates were (" << xIn << ',' << yIn << "). The calculated centroid was (" << x << ',' << y << ")." << endl;
	cout << "Error in pixel units = " << sqrt((x - xIn)*(x - xIn) + (y - yIn)*(y - yIn)) << endl << endl;
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
 * @param xPixels Number of pixels in the x-axis to bin the data into
 * @param yPixels Number of pixels in the y-axis to bin the data into
 * @param sampling Number of data points per pixel
 * @param time Integration time /s
 * @param area Area of telescope aperture /ms^-2
 * @param QE Quantum efficiency. Ideal 1.
 * @param temperature Temperature of the sensor /K. Ideal -> 0.
 * @param emissivity Emissivity of the sensor. Ideal 1.
 * @param readout Readout noise /electrons. Ideal 0.
 */
void runToFile(float xIn, float yIn, int xPixels, int yPixels, int sampling, float time, float area, float QE,
			   float temperature, float emissivity, int readout) {

	int xPoints = xPixels * sampling;
	int yPoints = yPixels * sampling;
	
	// For testing multiple runs, outputting to an output csv file
	ofstream outFile; // Initialise output file
	outFile.open("results.csv");

/*	outFile << "Input centre: (" << xIn << ';' << yIn << "), pixels in each dimension: " << xPixels << ';' 
			<< yPixels << ", data points simulated in each dimension: " << xPoints << "; " << yPoints << endl;

	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	outFile << "Running the simulation 10 times for a star position which varies between -0.5 to +0.5 from the input centre,," << endl;
	for (int i = 0; i < 10; i++) {

		Test* t = new Test(N, xIn + distribution(generator), yIn + distribution(generator), sigmaX, sigmaY, xPixels, yPixels, xPoints, yPoints);
		t->run(true, time, area); // Run with noise, with time and area
		float x = (t->xCentre * xPixels); // Convert output to pixels
		float y = (t->yCentre * yPixels);
		outFile << i << ',' << sqrt((x - xIn)*(x - xIn) + (y - yIn)*(y - yIn)) << ',' << x << ',' << y << endl;
		delete t;
	}
*/
	outFile << endl << "Varying sigma: " << endl;
	outFile << "Sigma in both dimensions, Distance, x-centre, y-centre" << endl;

	for (int mag = 10; mag <= 15; mag++) { // Run test varying magnitude
		outFile << endl << "Magnitude: " << mag << endl;
		for (float i = 10; i <= 100; i+=10) { // Run test varying sigma for each magnitude
			int N = pow(2.512, -1 * mag) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
			Test* t = new Test(N, xIn * sampling, yIn * sampling, i, i, xPixels, yPixels, xPoints, yPoints);
			t->run(false, time, area, QE, temperature, emissivity, readout); // Run with noise for input time and area 

			float x = (t->xCentre * xPixels);
			float y = (t->yCentre * yPixels);
			outFile << i << ',' << sqrt((x - xIn)*(x - xIn) + (y - yIn)*(y - yIn)) << ',' << x << ',' << y << endl;
			delete t;
		}
	}
	outFile.close();
}

///Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	//float magnitude = 14; // Star magnitude
	float xIn = 4.2; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 4.8;
	int xPixels = 100;
	int yPixels = 100;
	int sampling = 100; // Pixel sampling: Simulated points per pixel
	float exposureTime = 1;
	float area = 1;
	float QE = 1;
	float temperature = 273;
	float emissivity = 1;
	int readout = 0;

	//float N = pow(2.512, -1 * magnitude) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
	//runOnce(N, xIn, yIn, 10, 10, xPixels, yPixels, sampling, exposureTime, area, QE, temperature, emissivity, readout);
	
	runToFile(xIn, yIn, xPixels, yPixels, sampling, exposureTime, area, QE, temperature, emissivity, readout);

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
// DONE - Look at photons per second for a given magnitude. 
// DONE - Have the separate parameters of an input star magnitude and the integration time, keeping one constant and varying the other. 
// 1/(sampling frequency) = (exposure time + readout time). 
//
// DONE - Have parameters for things like quantum efficiency and dark current, instrument emissivity = 1. 
// DONE - Use a uniform distribution inside a pixel to define the starting position between simulated Gaussian2d points. 
// Relate sigma and mu in PSF
// DONE - Fix memory leak. 
// DONE - Define the edges as going from 0 to 1 in both dimensions, and then manipulate afterwards. 
// DONE - Play around with 1x1 and 2x2, etc small inputs. 
