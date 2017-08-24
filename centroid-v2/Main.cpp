/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Generate a 2d Gaussian with noise, bin it into "pixels" and find the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.1.3 24-08-2017
 */

#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

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
	//Test::print2dVector(t->pixelData);
	
	int sum = 0;
	for (vector<int> v: t->pixelData) {
		for (int i: v) sum += i;
	}
	cout << "Sum of detected photons = " << sum << '\a' << endl << endl;
	delete t;
}

/**
 * @brief Test multiple runs, outputting the results to a CSV file. 
 *
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

	outFile << "Input centre: (" << xIn << ';' << yIn << "), pixels in each dimension: " << xPixels << ';' 
			<< yPixels << ", data points simulated in each dimension: " << xPoints << "; " << yPoints << endl;

	std::default_random_engine generator; // Initialise uniform distribution and add to inputted x and y
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

//	outFile << "Running for a star position which varies between -0.5 to +0.5 from the input centre, sigma = 1," << endl;
//	for (int s = 1; s <= 20; s++) {
//		cout << "Calculating for sigma = " << s << "..." << endl;
//		for (float i = -0.5; i < 0.5; i += 0.1) {
//
//			int gaussianCentreX = (xIn + i) * sampling;
//			Test* t = new Test(532235, gaussianCentreX, yIn * sampling, s, s, xPixels, yPixels, xPoints, yPoints);
//			t->run(true, time, area, QE, temperature, emissivity, readout); // Run with noise, with time and area
//			float x = (t->xCentre * xPixels); // Convert output to pixels
//			float y = (t->yCentre * yPixels);
//			outFile << i << ',' << sqrt((x - (xIn + i))*(x - (xIn + i)) + (y - yIn)*(y - yIn)) << ',' << x << ',' << y << endl;
//			//Test::print2dVector(t->pixelData);
//			delete t;
//		}
//	}

	outFile << endl << "Varying sigma: " << endl;
	outFile << "Sigma in both dimensions, Distance, x-centre, y-centre" << endl;

	for (int mag = 7; mag <= 13; mag += 3) { // Run test varying magnitude
		outFile << endl << "Magnitude: " << mag << endl;
		for (float i = 1; i <= 10; i += 1) { // Run test varying sigma for each magnitude
			int N = pow(2.512, -1 * mag) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
			float uniformX = xIn + distribution(generator);
			float uniformY = yIn + distribution(generator);
			Test* t = new Test(N, uniformX * sampling, uniformY * sampling, i, i, xPixels, yPixels, xPoints, yPoints);
			t->run(true, time, area, QE, temperature, emissivity, readout); // Run with noise for input time and area 

			float x = (t->xCentre * xPixels);
			float y = (t->yCentre * yPixels);
			outFile << i << ',' << sqrt((x - uniformX)*(x - uniformX) + (y - uniformY)*(y - uniformY)) << ',' << x << ',' << y << endl;
			delete t;
		}
	}
	outFile.close();
}

///Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	//float magnitude = 12; // Star magnitude
	float xIn = 50;//; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 50;
	int xPixels = 100;
	int yPixels = 100;
	int sampling = 10; // Pixel sampling: Simulated points per pixel
	float exposureTime = 1;
	float area = 0.6362; // Entrance pupil diameter 450mm
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 1;
	int readout = 8;
	
	//float N = pow(2.512, -1 * magnitude) * 3.36E10; // Convert magnitude to photons s^-1 m^-2
	//runOnce(N, xIn, yIn, 10, 10, xPixels, yPixels, sampling, exposureTime, area, QE, temperature, emissivity, readout);
	
	runToFile(xIn, yIn, xPixels, yPixels, sampling, exposureTime, area, QE, temperature, emissivity, readout);

	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl;

	cout << '\a';
	return 0;
}

// TODO: 
// DONE - Read through the Twinkle documents to replace the 0 and 1 and 273 parameters in int main() with the actual values.
// DONE - (E2V CCD230-42) Pick a CCD detector and plug in the parameters. It can be changed later. Use back-illuminated. 
// Change output to be more easily graphed. 
// Make pixels vs simels consistent. Re-generate all the graphs generated to be consistent. 
// Re-plot error by sigma by magnitude. Try magnitudes 7, 10, 13. 
// Run Monte Carlo for that graph, finding an average. Maximum of error axis at 0.2. Make sure the plots are representative with averages and not just one run.
// Implement comparison of input photons and detected photons. 
// Output parameters simulated at beginning of run to console. 
// Create flowchart of I/O of the centroid simulation, going through all modular parts of the code.
// Magnitude -> W.m^-2 flux -> Add photon noise and background radiation -> Calculate number of photons -> Convert n_photons to n_photo-electrons
//			 -> Add dark current -> Multiply by exposure time -> Add readout noise from datasheet. 
// Include a ADU placeholder = 1 for digital readout. 
// Look up Jansky units. Claudio's spreadsheet converts magnitude to flux, and then flux to photons. Make sure the flux is in W.m^-2.Hz^-1. 
// Read some FGS literature to have a body of work that can be cited and referenced, with some problems already solved.
//
// Go back from number of photons to flux by multiplying by hv. Take the centre of the frequency band as the frequency. 
// (Level 2: Take spectral dependence on star temperature for frequency. )
// Include emissivity of dichroic and emissivity of beam splitter, ~0.1. 
//
// Level 0 - This project. 
// Level 1 - Taking a PNG image or FFTS as an input, or something. Or input a CSV, or something. Run calculation from that PSF. 
// Level 2 - Take into account telescope mirror abberations. 
