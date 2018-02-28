/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to take a 2d PSF from Zemax, add noise, bin it into "pixels" and find
 * the centroid of the binned data.
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 3.2.0 2018-02-22
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "MonteCarlo.hpp"

using namespace std;

/// Run the algorithm for a given file over the given magnitudes, outputting the average errors to file
void runFromTSV(ofstream &outputFile, string inFileName) {

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
	bool zodiacal = false;

	outputFile << inFileName << ',';
	MonteCarlo* m = new MonteCarlo(inFileName, xIn * 512 / xPixels, yIn * 512 / yPixels, xPixels, yPixels, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	vector<float> uncertainty;
	for (int mag = 7; mag <= 13; mag += 3) {
		outputFile << m->run(mag, mag, mag, 10, 1, true) << ',';
		uncertainty.push_back(m->uncertainty);
	}
	for (float f: uncertainty) outputFile << f << ',';
	delete m; // Close input file
	outputFile << endl;
}

/// Main method. Runs tests with inputted parameters.
int main() {
	
	time_t startTime = time(nullptr);
	cout << '\a' << endl << "Start time: " << asctime(localtime(&startTime)) << endl;
	cout << "NOTE: If out-of-bounds errors come up, make sure that the the input file is Unix format. " << endl << endl;
	
	ofstream outFile;
	outFile.open("results.csv");
	outFile << "Field number,Magnitude 7,Magnitude 10,Magnitude 13,SD 7,SD 10,SD 13" << endl;

	runFromTSV(outFile, "Zemax/Field1.tsv");
	runFromTSV(outFile, "Zemax/Field2.tsv");
	runFromTSV(outFile, "Zemax/Field3.tsv");
	runFromTSV(outFile, "Zemax/Field4.tsv");
	runFromTSV(outFile, "Zemax/Field5.tsv");
	runFromTSV(outFile, "Zemax/Field6.tsv");
	runFromTSV(outFile, "Zemax/Field7.tsv");
	runFromTSV(outFile, "Zemax/Field8.tsv");
	runFromTSV(outFile, "Zemax/Field9.tsv");
	runFromTSV(outFile, "Zemax/Field10-2.tsv");
	runFromTSV(outFile, "Zemax/Field11.tsv");
	runFromTSV(outFile, "Zemax/Field12.tsv");
	runFromTSV(outFile, "Zemax/Field13.tsv");
	runFromTSV(outFile, "Zemax/Field14.tsv");
	runFromTSV(outFile, "Zemax/Field15.tsv");
	runFromTSV(outFile, "Zemax/Field16.tsv");
	runFromTSV(outFile, "Zemax/Field17.tsv");
	
	outFile.close();

	time_t endTime  = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl << '\a';

	return 0;
}
