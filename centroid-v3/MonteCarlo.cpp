/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run Monte Carlo simulation of centroid determination error
 *
 * @file MonteCarlo.cpp
 * @brief Monte Carlo simulation of centroid determination error
 * @author Feiyu Fang
 * @version 3.0.0 2017-11-04
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "MonteCarlo.hpp"

using namespace std;

/**
 * Constructor to create a new MonteCarlo object with the input parameters. Opens a results file and outputs the parameters to the file.
 * @brief Constructs a MonteCarlo object with input parameters and opens file
 * 
 * @param inFileName Name of input file; "PSF-FFT-1024.tsv" preferred. 
 * @param outFileName Name of output file; "results.csv" preferred. 
 * @param inX x-coordinate of star
 * @param inY y-coordinate of star
 * @param horizPixels Number of pixels in the x-axis to bin the data into
 * @param vertPixels Number of pixels in the y-axis to bin the data into
 * @param t Integration time /s
 * @param diameter Diameter of telescope input pupil /m
 * @param qEff Quantum efficiency. Ideal 1.
 * @param temp Temperature of the sensor /K. Ideal -> 0.
 * @param e Emissivity of the dichroic. Ideal 1. 
 * @param readNoise Readout noise /electrons. Ideal 0. 
 * @param analogueDigitalUnits Analogue-to-digital units. Electrons per count; ideal 1.
 * @param darkCurrent Dark current for the sensor. Ideal 0. 
 * @param zodiac Whether to include zodiacal light as a background
 */
MonteCarlo::MonteCarlo(string inFileName, string outFileName, float inX, float inY, int horizPixels, int vertPixels, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits, float darkCurrent, bool zodiac) {
	xIn = inX;
	yIn = inY;
	xPixels = horizPixels;
	yPixels = vertPixels;
	time = t;
	area = M_PI * pow((diameter / 2.), 2);
	QE = qEff;
	temperature = temp;
	emissivity = e;
	readout = readNoise;
	ADU = analogueDigitalUnits;
	darkSignal = darkCurrent;
	zodiacal = zodiac;
	inputFile = inFileName;
	
	// Open file and output parameters
	outFile.open(outFileName);
	outFile << "Test: Input centre: (" << xIn << ';' << yIn << "). Pixels in each dimension: ("
			<< xPixels << ';' << yPixels << "). Exposure time: " << time << " s" << endl;
	outFile << "Telescope pupil area: " << area << " m^2. QE: " << QE << ". Temperature: " << temperature 
			<< " K. Emissivity of sensor: " << emissivity << ". Readout noise: " << readout << " electrons. " << endl;

//	outFile << endl << "Sigma in both dimensions, Average distance, Photons in, Photons detected, Monte Carlo standard deviation" << endl;
	outFile << endl << "X-coordinate, Y-coordinate" << endl;
}


/**
 * @brief Destructor for MonteCarlo object closes the current file
 */
MonteCarlo::~MonteCarlo() {outFile.close();}

/**
 * Private static function to return the total number of photons in a pixel grid, either of pixels or simels.
 *
 * @param matrix The pixel grid in a 2d int vector
 * @return Total number of photons in the inputted matrix
 */
int MonteCarlo::sumPhotons(vector<vector<int>> matrix) {
	
	int sum = 0;
	for (vector<int> v: matrix) {for (int i: v) sum += i;}
	return sum;
}

/**
 * Private static function to calculate averages
 * @brief Calculates the average of numbers held in a vector
 * @param in A vector<float> containing the integers whose average is to be found
 * @return Average number
 */
float MonteCarlo::average(vector<float> in) {

	float sum = accumulate(in.begin(), in.end(), 0.);
	return (sum / in.size());
}

/**
 * Private static function to calculate the standard deviation of a vector of floats
 * @brief Calculates the standard deviation of numbers held in a vector
 * @param in A vector<float> containing the numbers whose SD is to be found
 * @return Standard deviation
 */
float MonteCarlo::stdDev(vector<float> in) {
	
	float mean = average(in);
	float accum = 0.0;
	for_each (begin(in), end(in), [&](const float d) {accum += (d - mean) * (d - mean);});

	return sqrt(accum / (in.size()-1));
}

/**
 * Private function to simulate a star moving in the field of view. This changes the location of the star with
 * a Brownian movement and with a movement bias. 
 * @brief Move the star with Brownian motion with a bias
 *
 * @param biasDistance Bias distance for the star movement /arcsec
 * @param biasAngle Bias angle for star movement /deg
 * @param brownianRMS RMS of Brownian motion distance /arcsec
 */
void MonteCarlo::brownian(int biasDistance, int biasAngle, float brownianRMS) {

	// TODO: Scale the generated random variablesto their correct arcsec units.

	// Seed the generation of uniform-distributed random variable with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);

	uniform_int_distribution<int> uniformAngle(1, 180); // Uniform distribution of Brownian motion angle
	normal_distribution<double> brownianDistance(0, brownianRMS); // Normal distribution mean at 0, RMS 0.1", 1" or 10".
	float distance = brownianDistance(generator);
	float angle = uniformAngle(generator) * 180. / M_PI;
	xIn += distance * cos(angle); // Update the centre location after Brownian motion
	yIn += distance * sin(angle);

	xIn += biasDistance * cos(biasAngle * 180. / M_PI); // Update the centre location after star movement bias
	yIn += biasDistance * sin(biasAngle * 180. / M_PI);
}

/**
 * Run Monte Carlo simulation for star at a random position within a pixel with a given magnitude, running for n times
 * 
 * @brief Run Monte Carlo simulation
 * @param magB B-magnitude of the star to be tested
 * @param magV V-magnitude of the star to be tested
 * @param magR R-magnitude of the star to be tested
 * @param iterations Number of times to run simulation
 */
void MonteCarlo::run(float magB, float magV, float magR, int iterations, bool huygens) {

	outFile << endl << "B-magnitude: " << magB << "; V-magnitude: " << magV << "; R-magnitude: " << magR << endl;
	cout << "Calculating for B, V, R magnitudes = " << magB << ", " << magV << ", " << magR << " ..." << endl;
	vector<float> errors; // Vector to hold the error from each Monte Carlo simulation
	vector<float> photonsIn; 
	vector<float> photonsOut;

	int photonsB = Test::photonsInBand(magB, 'B'); // Convert magnitude in each band to photons s^-1 m^-2.
	int photonsV = Test::photonsInBand(magV, 'V');
	int photonsR = Test::photonsInBand(magR, 'R');
	int N = photonsB + photonsV + photonsR; // Total photons in all bands
	
	for (int j = 0; j < iterations; j++) { /// Iterate x times at random star positions and find average
		Test* t = new Test(N, xIn, yIn, xPixels, yPixels, zodiacal, inputFile);
		t->run(true, huygens, time, area, QE, temperature, emissivity, readout, ADU, darkSignal); // Run with noise for input time and area 

		float x = t->xCentre * xPixels;
		float y = t->yCentre * yPixels;
		errors.push_back(sqrt((x - xIn)*(x - xIn) + (y - yIn)*(y - yIn)));
		photonsIn.push_back(sumPhotons(t->photonsIn));
		photonsOut.push_back(sumPhotons(t->pixelData));
		outFile << x << ',' << y << endl;
		delete t;
	}
//	outFile << average(errors) << ',' << average(photonsIn) << ',' << average(photonsOut) << ',' << stdDev(errors) << endl;
}
