/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file MonteCarlo.hpp
 * @brief Header file for MonteCarlo class
 * @author Feiyu Fang
 * @version 3.0.0 2017-11-04
 */

#include <fstream>
#include "Test.hpp"

/**
 * @brief Class to run a Monte Carlo simulation of centroid determination error
 */
class MonteCarlo {
	public: 
		MonteCarlo(std::string inFileName, std::string outFileName, float inX, float inY, int horizPixels, int vertPixels, float t, float diameter, float qEff, float temp, float e, int readNoise, float analogueDigitalUnits, float darkCurrent, bool zodiac);
		~MonteCarlo();
		void run(float magB, float magV, float magR, int iterations, int brownianRuns, bool huygens);
		static std::vector<float> brownian(float biasDistance, int biasAngle, float brownianRMS, bool typeHuygens);

	private:
		float xIn, yIn, time, area, QE, temperature, emissivity, ADU, darkSignal;
		int xPixels, yPixels, readout;
		bool zodiacal;
		std::string inputFile;
		std::ofstream outFile;
		static int sumPhotons(std::vector<std::vector<int>>);
		static float average(std::vector<float>);
		static float stdDev(std::vector<float>);
};
