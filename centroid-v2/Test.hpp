/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Test.hpp
 * @brief Header file for Test class
 * @author Feiyu Fang
 * @version 2.1 28-07-2017
 */

#include "Gauss2d.hpp"

/**
 * @brief Class to bin an inputted Gaussian 2d array and find its centroid
 */
class Test {
	public:
		Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int xPoints, int yPoints);
		~Test();

		float xCentre;
		float yCentre;
		std::vector<std::vector<int>> pixelData;
		std::vector<std::vector<int>> gaussianInput; 
		
		void run(bool noise, float time, float area, float QE, float temperature, float emissivity, int readout);
		static void print2dVector(std::vector<std::vector<int>> data); 

	private:
		std::vector<std::vector<int>> noiseAfterBin; 
		float inX, inY, sigmaX, sigmaY;
		int N, horizPixels, vertPixels, pointsX, pointsY;

		static std::vector<int> sumVert(std::vector<std::vector<int>> in, int i, int end);
		std::vector<std::vector<int>> addPoissonNoise(float time, float area, float QE, float temperature, float emissivity, int readout);
		void binData(std::vector<std::vector<int>> dataIn, int h, int v);
		void findCentroid();
};
