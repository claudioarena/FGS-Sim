/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to generate a 2d Gaussian with noise, bin it into "pixels" and find 
 * the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 2.2 26-08-2017
 */

#include <chrono>
#include <iostream>

#include "MonteCarlo.hpp"

using namespace std;

///Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;

	float xIn = 500;//; // Input coordinates of defined centre in terms of pixels. 
	float yIn = 500;
	int xPixels = 1000;
	int yPixels = 1000;
	int sampling = 10; // Pixel sampling: Simulated points per pixel
	float exposureTime = 1;
	float diameter = 0.45; // Entrance pupil diameter /m. 450mm from Twinkle whitebook. 
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 1; // Proportion of input photons sent to FGS; parameter for dichroic. 
	int readout = 8;
	float ADU = 1;
	
	MonteCarlo* m = new MonteCarlo("results.csv", xIn, yIn, xPixels, yPixels, sampling, exposureTime, diameter, QE, temperature, emissivity, readout, ADU);
	for (int mag = 7; mag <= 13; mag += 3) {
		m->run(mag, 10); // Each Monte Carlo simulation uses the average of 10 runs
	}
	delete m; // Close output file

	time_t endTime  = time(nullptr);
	cout << endl << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl << '\a';

	return 0;
}

// TODO: 
// DONE - Read through the Twinkle documents to replace the 0 and 1 and 273 parameters in int main() with the actual values.
// DONE - (E2V CCD230-42) Pick a CCD detector and plug in the parameters. It can be changed later. Use back-illuminated. 
// Change output to be more easily graphed. 
// DONE - Make pixels vs simels consistent. Re-generate all the graphs generated to be consistent. 
// DONE - Re-plot error by sigma by magnitude. Try magnitudes 7, 10, 13. 
// Run Monte Carlo for that graph, finding an average. Maximum of error axis at 0.2. Make sure the plots are representative with averages and not just one run.
// DONE - Implement comparison of input photons and detected photons. 
// DONE - Output parameters simulated at beginning of run to console. 
// DONE - Create flowchart of I/O of the centroid simulation, going through all modular parts of the code.
// Magnitude -> W.m^-2 flux -> Add photon noise and background radiation -> Calculate number of photons -> Convert n_photons to n_photo-electrons
//			 -> Add dark current -> Multiply by exposure time -> Add readout noise from datasheet. 
// DONE - Include a ADU placeholder = 1 for digital readout. 
// Look up Jansky units. Claudio's spreadsheet converts magnitude to flux, and then flux to photons. Make sure the flux is in W.m^-2.Hz^-1. 
// Read some FGS literature to have a body of work that can be cited and referenced, with some problems already solved.
//
// Go back from number of photons to flux by multiplying by hv. Take the centre of the frequency band as the frequency. 
// (Level 2: Take spectral dependence on star temperature for frequency. )
// DONE - Include emissivity of dichroic and emissivity of beam splitter, ~0.1. 
//
// Level 0 - This project. 
// Level 1 - Taking a PNG image or FFTS as an input, or something. Or input a CSV, or something. Run calculation from that PSF. 
// Level 2 - Take into account telescope mirror abberations. 
