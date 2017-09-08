/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to generate a 2d Gaussian with noise, bin it into "pixels" and find 
 * the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.2.1E 07-09-2017
 */

#include <chrono>
#include <iostream>

#include "MonteCarlo.hpp"

using namespace std;

/// Run with the change of one input to find parametric dependence
void run(ofstream& out, float xIn, float yIn, int xPixels, int yPixels, int sampling, float exposureTime, float diameter, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal, bool zodiacal) {
	MonteCarlo* n = new MonteCarlo("results.csv", xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	for (int mag = 7; mag <= 13; mag += 3) {
		out << n->run(mag, mag, mag, 10) << ','; // Each Monte Carlo simulation uses the average of 10 runs
	}
	delete n; 
	out << endl;
}

/// Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	float xIn = 50; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 50;
	int xPixels = 100;
	int yPixels = 100;
	int sampling = 10; // Pixel sampling: Simulated points per pixel
	float exposureTime = 0.1; // Time /s
	float diameter = 0.45; // Entrance pupil diameter /m. 450mm from Twinkle whitebook. 
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 0.3; // Proportion of input photons sent to FGS; parameter for dichroic. 
	int readout = 8;
	float ADU = 1;
	float darkSignal = 0.2;
	bool zodiacal = false;
	
//	MonteCarlo* m = new MonteCarlo("results.csv", xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, 1, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
//	for (int mag = 7; mag <= 13; mag += 3) {
//		m->run(mag, mag, mag, 10);
//	}
//	delete m; // Close output file
	
	ofstream outFile;
	outFile.open("parameters.csv");
	
	cout << "Time" << endl;
	outFile << endl << "Varying time from 0.02 to 0.2 in 0.02 steps: " << endl;
	for (float i = 0.02; i <= 0.2; i += 0.02) run(outFile, xIn, yIn, xPixels, yPixels, sampling, i, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);

	cout << "Temperature" << endl;
	outFile << endl << "Varying temperature from 20K to 200K in 20K steps: " << endl;
	for (float i = 20; i <= 200; i += 20) run(outFile, xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, QE, i, emissivity, readout, ADU, darkSignal, zodiacal);

	cout << "QE/emissivity" << endl;
	outFile << endl << "Varying QE or emissivity from 0.1 to 1 in 0.1 steps: " << endl;
	for (float i = 20; i <= 200; i += 20) run(outFile, xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, i, temperature, emissivity, readout, ADU, darkSignal, zodiacal);

	cout << "Readout noise" << endl;
	outFile << endl << "Varying readout noise from 1 to 10 in 1 electron steps: " << endl;
	for (float i = 1; i <= 10; i++) run(outFile, xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, QE, temperature, emissivity, i, ADU, darkSignal, zodiacal);

	cout << "Dark signal" << endl;
	outFile << endl << "Varying dark signal from 0.1 to 1.0 in 0.1 electron steps: " << endl;
	for (float i = 0.1; i <= 1; i += 0.1) run(outFile, xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, i, zodiacal);

	outFile.close();

	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl << '\a';

	return 0;
}

// TODO: 
// Magnitude -> W.m^-2 flux -> Add photon noise and background radiation -> Calculate number of photons -> Convert n_photons to n_photo-electrons
//			 -> Add dark current -> Multiply by exposure time -> Add readout noise from datasheet. 
// Look up Jansky units. Claudio's spreadsheet converts magnitude to flux, and then flux to photons. Make sure the flux is in W.m^-2.Hz^-1. 
// Read some FGS literature to have a body of work that can be cited and referenced, with some problems already solved.
//
// DONE - Find out what noise is dominant. Photon Poisson noise, or dark current? Find the order of the noise. 
// DONE - Add emissivity of 0.01-0.03 for the temperature of the mirrors, where the mirror will also be emitting infrared photons. 
// DONE - Add contribution to background signal from zodiacal light, depending on the output of the Sun. 
// DONE - Introduce magnitude bands. Use J or K magnitudes for the Twinkle instrumentation. Possibly add multiple magnitude inputs for each band. 
// DONE - Use inputs of B-magnitude, V-magnitude, R-magnitude. (Next level: start from star temperature at a certain distance. )
// DONE - Use integration time of between 0.1 and 0.01 s. 
// DONE - Error bars in graphs, with standard deviation. 
// Try finding a parametric curve for the graphs, depending on time, star magnitude, etc. This means that we wouldn't need to find 100 Monte Carlos. 
//
// Go back from number of photons to flux by multiplying by hv. Take the centre of the frequency band as the frequency. 
// (Level 2: Take spectral dependence on star temperature for frequency. )
// DONE - Include emissivity of dichroic and emissivity of beam splitter, ~0.1. 
//
// Level 0 - This project. 
// Level 1 - Taking a PNG image or FFTS as an input, or something. Or input a CSV, or something. Run calculation from that PSF. 
// Level 2 - Take into account telescope mirror abberations. 
