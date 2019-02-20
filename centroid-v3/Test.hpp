/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Test.hpp
 * @brief Header file for Test class
 * @author Feiyu Fang
 * @version 3.1.0 2017-12-13
 */

#include "Brownian.hpp"
#include "PSF.hpp"
#include "Config/parameters.h"

/**
 * Header file for Test class. Takes a photon matrix, bins it into pixels and calculates its centroid. Currently 
 * set up to handle PSFs; comment PSF lines and uncomment Gaussian lines as marked here to use simulated 
 * Gaussian matrices instead of PSFs. 
 *
 * @brief Class to bin an inputted Gaussian 2d array and find its centroid
 */
class Test
{
  public:
	//Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int xPoints, int yPoints, bool zodiac); // Constructor for Gaussian
	Test(int nPhotons, float inX, float inY, int hPixels, int vPixels, bool zodiac, std::string name); // Constructor for PSF
	~Test();

	float xCentre;
	float yCentre;
	std::vector<std::vector<int>> pixelData;
	std::vector<std::vector<int>> simelsIn;

	void run(bool noise, bool huygens, float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal, Brownian *motion, int brownianRuns);
	static void print2dVector(std::vector<std::vector<int>> data);
	static double photonsInAllBands(std::vector<double> mags, std::vector<filter> fltrs);
	static double photonsInBand(float mag, struct filter flt);
	static std::vector<std::vector<int>> addMatrices(std::vector<std::vector<int>> a, std::vector<std::vector<int>> b);

	static float airmass(float alt);
	static float extinctionInMags(float alt);
	static float extinctionInPercentage(float alt);

  private:
	std::vector<std::vector<int>> noiseAfterBin;
	float xIn, yIn;					// PSF version
	int N, horizPixels, vertPixels; // PSF version
	//int xIn, yIn, sigmaX, sigmaY;					  // Gaussian version
	//int N, horizPixels, vertPixels, pointsX, pointsY; // Gaussian version
	bool zodiacal;
	std::string filename;
	Brownian *motion;

	static std::vector<int> sumVert(std::vector<std::vector<int>> in, int i, int end);
	static int mirrorThermalNoise(float area, float temperature);
	std::vector<std::vector<int>> addNoise(float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal);
	void binData(std::vector<std::vector<int>> dataIn, int h, int v);
	void findCentroid();
};
