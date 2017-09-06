/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file MonteCarlo.hpp
 * @brief Header file for MonteCarlo class
 * @author Feiyu Fang
 * @version 2.2 26-08-2017
 */

#include <fstream>
#include "Test.hpp"

class MonteCarlo {
	public: 
		MonteCarlo(std::string fileName, float inX, float inY, int horizPixels, int vertPixels, int samp, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits);
		~MonteCarlo();
		void run(float mag, int iterations);

	private:
		float xIn, yIn, time, area, QE, temperature, emissivity, ADU;
		int xPixels, yPixels, sampling, readout, xPoints, yPoints;
		std::ofstream outFile;
		static int sumPhotons(std::vector<std::vector<int>>);
		static float average(std::vector<float>);
		static float stdDev(std::vector<float>);
};
