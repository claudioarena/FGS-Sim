/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Take a Zemax PSF .txt file and extract its data into a vector<vector<int>> matrix. 
 * NOTE: The input .txt file must be converted from MS-DOS to Unix encoding; dos2unix is confirmed as working.
 *
 * @file PSF.cpp
 * @brief Imports a PSF from a Zemax .txt output
 * @author Feiyu Fang
 * @version 3.0.0 2017-11-03
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "PSF.hpp"

using namespace std;

/**
 * Constructor for PSF class. Imports the data from the file and normalises it according to the total photons.
 *
 * @brief Constructor imports and normalises data
 * @param name Input file name
 * @param N Number of photons to be distributed
 */
PSF::PSF(string name, int N) {
	filename = name;
	nPhotons = N;
	import();
	normalise();
}
PSF::~PSF() {}

/**
 * Private function to import the Zemax .txt file into this object's matrixIn member variable. 
 * @brief Imports the Zemax data
 */
void PSF::import() {

	ifstream file(filename);
	string line;

	for (int i = 0; i < 18; i++) getline(file, line); // Ignore lines in header. 21 for Huygens, 18 for FFT. 
	vector<vector<float>> out; // Output matrix for export
	
	while (getline(file, line)) { // Read each line into a stream
		stringstream linestream(line);
		float buffer;

		vector<float> numbersList; // Initialise row to be put into the output matrix
		while (linestream >> buffer) numbersList.push_back(buffer); // Read each token into the row vector<float>
		out.push_back(numbersList);
	}
	matrixIn = out;
}

/**
 * Private function to calculate the sum of all the unnormalised points in the Zemax matrix
 * 
 * @brief Sums all the points of the matrix in this object
 * @return Total sum
 */
float PSF::sum() {
	float out = 0;
	for (vector<float> v: matrixIn) for (float f: v) out += f;
	return out;
}

/**
 * Private function to normalise the inputted matrix and convert into the right number of quantised photons. 
 * 
 * @brief Normalise the matrix for output
 */
void PSF::normalise() {
	float factor = nPhotons / sum(); // Normalising factor: Divide by unnormalised sum, multiply by number of photons. 

	for (vector<float> v: matrixIn) {
		vector<int> row;
		for (float f: v) row.push_back(f * factor);
		matrixOut.push_back(row);
	}
}

/**
 * @brief Getter function for outputting the matrix
 *
 * @return Matrix of photons
 */
vector<vector<int>> PSF::getMatrix() {return matrixOut;}
