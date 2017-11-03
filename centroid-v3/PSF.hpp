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
		PSF(std::string name, int N);
		~PSF();
		std::vector<std::vector<int>> getMatrix();
		
	private: 
		void import();
		float sum();
		void normalise();

		std::vector<std::vector<float>> matrixIn;
		std::vector<std::vector<int>> matrixOut;
		std::string filename;
		int nPhotons;
};
