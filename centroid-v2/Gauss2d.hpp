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
		Gauss2d(int points, float cx, float cy, float sdX, float sdY); 
		
		int h;
		float centreX;
		float centreY;
		float sigmaX;
		float sigmaY;

		~Gauss2d() ;

		std::vector<std::vector<float>> generate();
	
	private:
		float gaussDist(float x, float mu, float sigma);

};
