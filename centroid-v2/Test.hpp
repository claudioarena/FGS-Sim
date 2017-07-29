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
		Test(int nPhotons, float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int points);
		~Test();

		float xCentre;
		float yCentre;
		std::vector<std::vector<int>> pixelData;
		
		void run(bool noise, float time, float area);
		static void print2dVector(std::vector<std::vector<int>> data); 

	private:
		std::vector<std::vector<int>> noiseAfterBin; 
		std::vector<std::vector<int>> gaussianInput; 
		float inX, inY, sigmaX, sigmaY;
		int N, horizPixels, vertPixels, h;

		static std::vector<float> sumVert(std::vector<std::vector<float>> in, int i, int end);
		static std::vector<int> sumVert(std::vector<std::vector<int>> in, int i, int end);
		std::vector<std::vector<int>> addPoissonNoise(float time, float area);
		static std::vector<std::vector<int>> binData(std::vector<std::vector<int>> dataIn, int h, int v);
		void findCentroid();
};
