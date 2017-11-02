/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Gauss2d.hpp
 * @brief Header for Gauss2d class
 * @author Feiyu Fang
 * @version 2.1 28-07-2017
 */
#include <iostream>
#include <vector>

/**
 * @brief Generate a 2d Gaussian. 
 */
class Gauss2d {

	public: 
		Gauss2d(int nPhotons, int xPoints, int yPoints, float cx, float cy, float sdX, float sdY); 
		~Gauss2d() ;
		
		float centreX;
		float centreY;

		std::vector<std::vector<int>> generate();

	private:
		int N;
		int hX;
		int hY;
		float sigmaX;
		float sigmaY;

		static std::vector<int> generateIntVector(int N, int bins, float mean, float sigma);
		static float normalCDF(float a, float b, float mean, float sigma);
};
