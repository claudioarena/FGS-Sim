/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file PSF.hpp
 * @brief Header for PSF class
 * @author Feiyu Fang
 * @version 3.0.0 2017-11-03
 */
#include <string>
#include <vector>

/**
 * @brief Import a PSF from a Zemax .txt file
 */
class PSF {
	
	public:
		PSF(std::string name, int N, bool h);
		~PSF();
		std::vector<std::vector<int>> samplePhotons(int xCentre, int yCentre);
		static float sum(std::vector<std::vector<float>> in);
		static int sum(std::vector<std::vector<int>> in);
		
	private: 
		void import();
		void normalise();
		static std::vector<int> shiftRow(int shift, std::vector<int> row);

		std::vector<std::vector<float>> matrixIn;
		std::vector<std::vector<int>> matrixNormalised;
		std::string filename;
		int nPhotons;
		int headerLines;
};
