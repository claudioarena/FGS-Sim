/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file MonteCarlo.hpp
 * @brief Header file for MonteCarlo class
 * @author Feiyu Fang
 * @version 2.2 26-08-2017
 */

#include "Test.hpp"

class MonteCarlo {
	public: 
		MonteCarlo(float inX, float inY, int horizPixels, int vertPixels, int samp, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits);
		~MonteCarlo();
		void runToFile(std::string fileName);

	private:
		float xIn, yIn, time, area, QE, temperature, emissivity, ADU;
		int xPixels, yPixels, sampling, readout;
		static int sumPhotons(std::vector<std::vector<int>>);
};
