/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to generate a 2d PSF with noise, bin it into "pixels" and find the
 * centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 3.1.0 2017-12-13
 */

#include <chrono>
#include <iostream>

#include "MonteCarlo.hpp"

using namespace std;

/// Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;
	cout << "NOTE: If out-of-bounds errors come up, make sure that the the input file is Unix format. " << endl << endl;

	float xIn = 32; // Input coordinates of defined centre in terms of pixel coordinates.
	float yIn = 32;
	int xPixels = 128;
	int yPixels = 128;
	float exposureTime = 0.1; // Time /s
	float diameter = 0.45; // Entrance pupil diameter /m. 450mm from Twinkle whitebook. 
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 0.3; // Proportion of input photons sent to FGS; parameter for dichroic. 
	int readout = 1;
	float ADU = 1;
	float darkSignal = 0.2;
	bool zodiacal = true;
	
	MonteCarlo* m = new MonteCarlo("Zemax/Field1.tsv", "results.csv", xIn * 512 / xPixels, yIn * 512 / yPixels, xPixels, yPixels, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	for (int mag = 7; mag <= 13; mag += 3) {
		m->run(mag, mag, mag, 10, 1, true); 
	}
	delete m; // Close output file
	
	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl << '\a';

	return 0;
}
