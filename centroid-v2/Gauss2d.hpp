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
		Gauss2d(int xPoints, int yPoints, float cx, float cy, float sdX, float sdY); 
		
		int hX;
		int hY;
		float centreX;
		float centreY;
		float sigmaX;
		float sigmaY;

		~Gauss2d() ;

		std::vector<std::vector<float>> generate();
		std::vector<std::vector<int>> generateData(int N);

	private:
		float gaussDist(float x, float mu, float sigma);
		int integrateBetween(float a, float b, float mu, float sigma, int N);
		static double normalCDF(double x, double mean, double sigma);

};
