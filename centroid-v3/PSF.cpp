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
 * @param h Type of PSF. True for Huygens, false for FFT. 
 */
PSF::PSF(string name, int N, bool h)
{
	filename = name;
	nPhotons = N;
	if (h == true)
		headerLines = 21;
	else
		headerLines = 18;
	import();
	normalise();
}
PSF::~PSF() {}

/**
 * Private function to import the Zemax .txt file into this object's matrixIn member variable. 
 * @brief Imports the Zemax data
 */
void PSF::import()
{

	ifstream file(filename);
	string line;

	for (int i = 0; i < headerLines; i++)
		getline(file, line);   // Ignore lines in header. 21 for Huygens, 18 for FFT.
	vector<vector<float>> out; // Output matrix for export

	while (getline(file, line))
	{ // Read each line into a stream
		stringstream linestream(line);
		float buffer;

		vector<float> numbersList; // Initialise row to be put into the output matrix
		while (linestream >> buffer)
			numbersList.push_back(buffer); // Read each token into the row vector<float>
		out.push_back(numbersList);
	}
	matrixIn = out;
}

/**
 * Public static function to calculate the sum of all the elements in a matrix. 
 * Overloaded to use floats for unnormalised points in the Zemax matrix, or integers for counting photons. 
 * 
 * @brief Sums all the points of the matrix in this object
 * @return Total sum
 */
float PSF::sum(vector<vector<float>> in)
{
	float out = 0;
	for (vector<float> v : in)
		for (float f : v)
			out += f;
	return out;
}
int PSF::sum(vector<vector<int>> in)
{
	int out = 0;
	for (vector<int> v : in)
		for (int f : v)
			out += f;
	return out;
}

/**
 * Private function to normalise the inputted matrix and convert into the right number of quantised photons. 
 * 
 * @brief Normalise the matrix for output
 */
void PSF::normalise()
{
	float factor = nPhotons / sum(matrixIn); // Normalising factor: Divide by unnormalised sum, multiply by number of photons.

	for (vector<float> v : matrixIn)
	{
		vector<int> row;
		for (float f : v)
			row.push_back(f * factor);
		matrixNormalised.push_back(row);
	}
}

/**
 * Private static function to shift a vector of integers left or right, replacing the vacated spaces with 0, for moving the PSF around. 
 * Positive shift is to the right; negative shift is to the left. 
 *
 * @brief Shift integers left or right within a vector
 *
 * @param shift Amount to shift the numbers by
 * @param row Vector to be shifted
 * @return Shifted 1d vector
 */
vector<int> PSF::shiftRow(int shift, vector<int> row)
{
	vector<int> out;

	if (shift < 0)
	{ // Shift to the left if negative shift
		for (unsigned i = 0; i < (row.size() + shift); i++)
			out.push_back(row.at(i - shift));
		while (out.size() != row.size())
			out.push_back(0);
	}
	else
	{ // Shift to the right if positive shift
		while (out.size() != (unsigned)shift)
			out.push_back(0);
		for (unsigned i = shift; i < row.size(); i++)
			out.push_back(row.at(i - shift));
	}

	return out;
}

/**
 * The normalised matrix of photons is always centred, so this function samples from it to output a PSF matrix centred at the given coordinates. 
 * Some photons are not detected to one side, and some simels are empty on the other side and have 0 values. 
 *
 * @brief Sample the PSF with given centre coordinates.
 *
 * @param xCentre Simel x-coordinate of PSF centre
 * @param yCentre Simel y-coordinate of PSF centre
 * @return Matrix of photons with the PSF centred at the given coordinates. 
 */
vector<vector<int>> PSF::samplePhotons(int xCentre, int yCentre)
{

	unsigned width = matrixNormalised.at(0).size(); // Height and width of the simel matrix
	unsigned height = matrixNormalised.size();

	int shiftX = xCentre - (width / 2); // Number of simels to move the data horizontally and vertically across
	int shiftY = yCentre - (height / 2);
	vector<int> zeroRow; // Row of zeros to replace rows that have been moved but not replaced
	while (zeroRow.size() != width)
		zeroRow.push_back(0);

	vector<vector<int>> out;
	if (shiftY < 0)
	{ // Shift matrix upwards for negative shift
		for (unsigned i = (-1 * shiftY); i < height; i++)
			out.push_back(shiftRow(shiftX, matrixNormalised.at(i)));
		while (out.size() != height)
			out.push_back(zeroRow);
	}
	else
	{ // Shift matrix downwards for positive shift
		while (out.size() != (unsigned)shiftY)
			out.push_back(zeroRow);
		for (unsigned i = shiftY; i < height; i++)
			out.push_back(shiftRow(shiftX, matrixNormalised.at(i - shiftY)));
	}
	return out;
}
