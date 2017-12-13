/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file Brownian.hpp
 * @brief Header file for Brownian class
 * @author Feiyu Fang
 * @version 3.0.3 2017-12-13
 */

class Brownian {
	public:
		Brownian(float dist, int angle, float rms, bool type);
		~Brownian();
		void generate();
		void reset();
		float brownianDx, brownianDy; 

	private:
		float biasDistance, brownianRMS, distance, angle;
		int biasAngle;
		bool typeHuygens;

};
