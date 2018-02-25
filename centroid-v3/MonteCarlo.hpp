/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file MonteCarlo.hpp
 * @brief Header file for MonteCarlo class
 * @author Feiyu Fang
 * @version 3.2.0 2018-02-22
 */

#include "Test.hpp"

/**
 * @brief Class to run a Monte Carlo simulation of centroid determination error
 */
class MonteCarlo {
	public: 
		MonteCarlo(std::string inFileName, float inX, float inY, int horizPixels, int vertPixels, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits, float darkCurrent, bool zodiac);
		~MonteCarlo();
		float run(float magB, float magV, float magR, int iterations, int brownianRuns, bool huygens);
		static std::vector<float> brownian(float biasDistance, int biasAngle, float brownianRMS, bool typeHuygens);
		float uncertainty;

	private:
		float xIn, yIn, time, area, QE, temperature, emissivity, ADU, darkSignal;
		int xPixels, yPixels, readout;
		bool zodiacal;
		std::string inputFile;
		static int sumPhotons(std::vector<std::vector<int>>);
		static float average(std::vector<float>);
		static float stdDev(std::vector<float>);
};
