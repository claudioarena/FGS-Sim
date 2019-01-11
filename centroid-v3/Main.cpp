/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to take a 2d PSF from Zemax, add noise, bin it into "pixels" and find
 * the centroid of the binned data.
 * NOTE: A TSV saved from Zemax has MS-DOS line endings and will need to be converted to Unix format. On Unix, 
 * the dos2unix utility is confirmed as working. 
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 3.2.1 2018-03-21
 */
#include "Config\parameters.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include "MonteCarlo.hpp"
#include <iomanip>

// for convenience
using json = nlohmann::json;

using namespace std;

/// Run the algorithm for a given file over the given magnitudes, outputting the average errors to file
void runFromTSV(ofstream &outputFile, std::string inFileName)
{

	float xIn = 64; // Input coordinates of defined centre in terms of pixel coordinates.
	float yIn = 64;
	int xPixels = 128;
	int yPixels = 128;
	float exposureTime = 1.0; // Time /s
	float diameter = 0.45;	// Entrance pupil diameter /m. 450mm from Twinkle whitebook.
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 0.3; // Proportion of input photons sent to FGS; parameter for dichroic.
	int readout = 1;
	float ADU = 1;
	float darkSignal = 0.2; // Dark signal at 293 K. Scaling of dark signal is with the equation in the E2V CCD230-42 datasheet.
	bool zodiacal = false;

	outputFile << inFileName << ','; // This line is helpful for making the output CSV make more sense when testing multiple files at once.
	// TODO: Remove hard-coded 512 in line below. The 512 factors are to scale by the 512x512 pixel sampling from Zemax.
	MonteCarlo *m = new MonteCarlo(inFileName, xIn * 512.0 / xPixels, yIn * 512.0 / yPixels, xPixels, yPixels, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	vector<float> uncertainty;
	for (int mag = 7; mag <= 16; mag += 1)
	{
		outputFile << m->run(mag, mag, mag, 10, 1, true) << ','; // Output values for each magnitude
		uncertainty.push_back(m->uncertainty);
	}
	for (float f : uncertainty)
		outputFile << f << ','; // Output the uncertainties after the values in the same row
	delete m;
	outputFile << endl;
}

/// Main method. Runs tests for given TSV input files from Zemax.
int main()
{

	time_t startTime = time(nullptr);
	cout << '\a' << endl
		 << "Start time: " << asctime(localtime(&startTime)) << endl;
	cout << "NOTE: If out-of-bounds errors come up, make sure that the the input file has Unix format line endings. " << endl
		 << endl;

	cout << "Configuration used: " << CONFIG << endl;
	cout << "diameter: " << TEL_DIAM << " mm" << endl;
	cout << endl;

	ofstream outFile;
	outFile.open("results2.csv");
	outFile << "Field number,Magnitude 7,Magnitude 10,Magnitude 13,SD 7,SD 10,SD 13" << endl;

	//runFromTSV(outFile, "Zemax/PSF-FFT-1024.tsv");
	runFromTSV(outFile, "Zemax/Field1.tsv");
	outFile.close();

	time_t endTime = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl
		 << '\a';

	return 0;
}

int newmain()
{
	struct Telescope
	{
		float diameter;
		float totalEfficiency;
	};

	struct FGSCamera
	{
		float DarkCurrent;
		float gain;
	};

	struct Telescope Twinkle;
	Twinkle.diameter = 0.45;
	Twinkle.totalEfficiency = 1;

	// create an empty structure (null)
	std::ifstream i("centroid-v3//test.json");
	json j;
	i >> j;
	int v = j["version"].get<int>();
	cout << v;
	cout << "Diameter: " << Twinkle.diameter << endl;

	return 0;
}
