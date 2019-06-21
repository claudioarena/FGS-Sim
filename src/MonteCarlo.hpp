/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file MonteCarlo.hpp
 * @brief Header file for MonteCarlo class
 * @author Feiyu Fang
 * @version 3.2.0 2018-02-22
 */

#include "Test.hpp"
#include "telescopes.h"
#include <memory>
#include "Frame.hpp"
#include <mutex>

struct FrameParameters
{
	double magB, magV, magR, star_fwhm_x, star_fwhm_y;
	std::vector<pixel_coordinates> input_coordinates, centroid_coordinates;
};

/**
 * @brief Class to run a Monte Carlo simulation of centroid determination error
 */
class MonteCarlo
{
public:
	MonteCarlo(Telescope _telescope, double _expTime, std::string _outFileName);
	~MonteCarlo();

	void run(std::vector<double> mags, std::vector<double> star_fwhm, std::vector<pixel_coordinates> coordinates, bool verbose)
	{
		run(mags, mags, mags, star_fwhm, star_fwhm, coordinates, verbose);
	}; //TODO: same coordinates each loop. Is that a problem?

	void run(std::vector<double> mags, std::vector<double> star_fwhm_x, std::vector<double> star_fwhm_y, std::vector<pixel_coordinates> coordinates, bool verbose)
	{
		run(mags, mags, mags, star_fwhm_x, star_fwhm_y, coordinates, verbose);
	};

	void run(std::vector<double> magB, std::vector<double> magV, std::vector<double> magR, std::vector<double> star_fwhm, std::vector<pixel_coordinates> coordinates, bool verbose)
	{
		run(magB, magV, magR, star_fwhm, star_fwhm, coordinates, verbose);
	};

	void run(std::vector<double> magB, std::vector<double> magV, std::vector<double> magR, std::vector<double> star_fwhm_x, std::vector<double> star_fwhm_y, std::vector<pixel_coordinates> coordinates, bool verbose);

private:
	Telescope tel;
	double expTime;
	//std::unique_ptr<Frame> frame;
	std::string outFileName;
	std::mutex params_mutex; // protects params vectors

	std::vector<FrameParameters> parametersVector(std::vector<double> magB, std::vector<double> magV, std::vector<double> magR, std::vector<double> star_fwhm_x, std::vector<double> star_fwhm_y, std::vector<pixel_coordinates> coordinates);

	void printFrameParametersVector(FrameParameters &parameter, FILE *outFile = NULL, bool verbose = false);
	void saveToFile(std::vector<FrameParameters> &parameters, bool verbose = false);

	void runThread(std::vector<FrameParameters> &params_v_in, std::vector<FrameParameters> &params_v_out);
};
