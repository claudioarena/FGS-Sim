/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Test.hpp
 * @brief Header file for Test class
 * @author Feiyu Fang
 * @version 2.1.3 24-08-2017
 */

//#include "Gauss2d.hpp"
#include "PSF.hpp"

/**
 * @brief Class to bin an inputted Gaussian 2d array and find its centroid
 */
class Test {
	public:
		//Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int xPoints, int yPoints, bool zodiac);
		Test(int nPhotons, int inX, int inY, int hPixels, int vPixels, bool zodiac, std::string name);
		~Test();

		float xCentre;
		float yCentre;
		std::vector<std::vector<int>> pixelData;
		std::vector<std::vector<int>> photonsIn; 
		
		void run(bool noise, float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal);
		static void print2dVector(std::vector<std::vector<int>> data); 
		static int photonsInBand(float mag, char band);

	private:
		std::vector<std::vector<int>> noiseAfterBin; 
		int xIn, yIn; //, sigmaX, sigmaY;
		int N, horizPixels, vertPixels; //, pointsX, pointsY;
		bool zodiacal;
		std::string filename;

		static std::vector<int> sumVert(std::vector<std::vector<int>> in, int i, int end);
		static int mirrorThermalNoise(float area, float temperature);
		std::vector<std::vector<int>> addNoise(float time, float area, float QE, float temperature, float emissivity, int readout, float ADU, float darkSignal);
		void binData(std::vector<std::vector<int>> dataIn, int h, int v);
		void findCentroid();
};
