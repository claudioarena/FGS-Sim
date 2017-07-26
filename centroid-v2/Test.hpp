/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Test.hpp
 * @brief Header file for Test class
 * @author Feiyu Fang
 * @version 2.0.1 25-07-2017
 */

#include "Gauss2d.hpp"

/**
 * @brief Class to bin an inputted Gaussian 2d array and find its centroid
 */
class Test {
	public:
		Test(float xIn, float yIn, float sdX, float sdY, int hPixels, int vPixels, int points);
		~Test();

		float xCentre;
		float yCentre;
		
		void run(bool noise, float time, float area);
		static std::vector<std::vector<int>> binData(std::vector<std::vector<float>> dataIn, int h, int v);
		static void print2dVector(std::vector<std::vector<int>> data, bool RMS); 
	private:

		std::vector<std::vector<int>> pixelData;
		std::vector<std::vector<int>> noiseAfterBin; 
		std::vector<std::vector<float>> gaussianInput; 

		float inX, inY, sigmaX, sigmaY;
		int horizPixels, vertPixels, h;

		static std::vector<float> sumVert(std::vector<std::vector<float>> in, int i, int end);
		static std::vector<int> sumVert(std::vector<std::vector<int>> in, int i, int end);
		std::vector<std::vector<int>> addPoissonNoise(float time, float area);
		void findCentroid();
};
