/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to generate a 2d Gaussian with noise, bin it into "pixels" and find 
 * the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.2.1 07-09-2017
 */

#include <chrono>
#include <iostream>

#include "MonteCarlo.hpp"

using namespace std;

/// Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	float xIn = 512; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 512;
	int xPixels = 128;
	int yPixels = 128;
	float exposureTime = 0.1; // Time /s
	float diameter = 0.45; // Entrance pupil diameter /m. 450mm from Twinkle whitebook. 
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 0.3; // Proportion of input photons sent to FGS; parameter for dichroic. 
	int readout = 8;
	float ADU = 1;
	float darkSignal = 0.2;
	bool zodiacal = true;
	
	MonteCarlo* m = new MonteCarlo("PSF-FFT-1024.tsv", "results.csv", xIn, yIn, xPixels, yPixels, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	for (int mag = 7; mag <= 13; mag += 3) {
		m->run(mag, mag, mag, 10);
	}
	delete m; // Close output file
	
	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl << '\a';

	return 0;
}
