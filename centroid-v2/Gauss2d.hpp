/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Gauss2d.hpp
 * @brief Header for Gauss2d class
 * @author Feiyu Fang
 * @version 2.0.1 24-07-2017
 */
#include <iostream>
#include <vector>

class Gauss2d {

	public: 
		Gauss2d(int nPhotons, int xPoints, int yPoints, float cx, float cy, float sdX, float sdY); 
		
		int N;
		int hX;
		int hY;
		float centreX;
		float centreY;
		float sigmaX;
		float sigmaY;

		~Gauss2d() ;

		std::vector<std::vector<float>> generate();
		static std::vector<int> generateIntVector(int N, int bins, float mean, float sigma);
		std::vector<std::vector<int>> generateIntGaussian();
	private:
		float gaussDist(float x, float mu, float sigma);
		static float normalCDF(float a, float b, float mean, float sigma);

};
