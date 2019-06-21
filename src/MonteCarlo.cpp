/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run Monte Carlo simulation of centroid determination error
 *
 * @file MonteCarlo.cpp
 * @brief Monte Carlo simulation of centroid determination error
 * @author Feiyu Fang
 * @version 3.2.0 2018-02-22
 */

#include "MonteCarlo.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <numeric>
#include <iostream>
#include <fstream>
#include "astroUtilities.hpp"
#include "FrameProcessor.hpp"
#include "typedefs.h"
#include <thread>

#define PRINT_INFO

/** TODO: rewrite documentation
 * Constructor to create a new MonteCarlo object with the input parameters. Opens a file containing a PSF.
 * Initialises file output if block at end is uncommented. 
 * @brief Constructs a MonteCarlo object with input parameters and opens file
 * 
 * @param inFileName Name of input file; "PSF-FFT-1024.tsv" preferred. 
 * @param inX x-coordinate of star
 * @param inY y-coordinate of star
 * @param horizPixels Number of pixels in the x-axis to bin the data into
 * @param vertPixels Number of pixels in the y-axis to bin the data into
 * @param t Integration time /s
 * @param diameter Diameter of telescope input pupil /m
 * @param qEff Quantum efficiency. Ideal 1.
 * @param temp Temperature of the sensor /K. Ideal -> 0.
 * @param e Emissivity of the dichroic. Ideal 1. 
 * @param readNoise Readout noise /electrons. Ideal 0. 
 * @param analogueDigitalUnits Analogue-to-digital units. Electrons per count; ideal 1.
 * @param darkCurrent Dark current for the sensor. Ideal 0. 
 * @param zodiac Whether to include zodiacal light as a background
 */
MonteCarlo::MonteCarlo(Telescope _telescope, double _expTime, std::string _outFileName)
	: tel(_telescope), expTime(_expTime), outFileName(_outFileName)
{
	//frame = std::make_unique<Frame>(_telescope, _expTime);
}

/**
 * @brief Destructor for MonteCarlo object closes the current file if uncommented
 */
MonteCarlo::~MonteCarlo()
{ /*outFile.close();*/
}

std::vector<FrameParameters> MonteCarlo::parametersVector(std::vector<double> magB, std::vector<double> magV, std::vector<double> magR, std::vector<double> star_fwhm_x, std::vector<double> star_fwhm_y, std::vector<pixel_coordinates> coordinates)
{
	std::vector<std::size_t> sizes{magB.size(), magV.size(), magR.size()};
	std::vector<std::size_t> sizes2{star_fwhm_x.size(), star_fwhm_y.size()};

	if ((astroUtilities::vectorSizes(sizes)) == false || (astroUtilities::vectorSizes(sizes2)) == false)
	{
		throw "Vector Sizes different for MonteCarlo simulations";
	}

	std::size_t size = (magB.size() * star_fwhm_x.size());
	FrameParameters parameters;
	std::vector<FrameParameters> parameters_v(size, parameters);

	//pixel 0,0
	pixel_coordinates zero_coord = pixel_coordinates{0.0, 0.0};
	std::vector<pixel_coordinates> zero_coord_v = std::vector<pixel_coordinates>(coordinates.size(), zero_coord);

	for (uint32_t i = 0; i < magB.size(); i++)
	{
		for (uint32_t j = 0; j < star_fwhm_x.size(); j++)
		{

			parameters.magB = magB.at(i);
			parameters.magV = magV.at(i);
			parameters.magR = magR.at(i);
			parameters.star_fwhm_x = star_fwhm_x.at(j);
			parameters.star_fwhm_y = star_fwhm_y.at(j);
			parameters.input_coordinates = coordinates;
			parameters.centroid_coordinates = zero_coord_v;
			parameters_v.at(j + star_fwhm_x.size() * i) = parameters;
		}
	}

	return parameters_v;
}

void MonteCarlo::printFrameParametersVector(FrameParameters &parameter, FILE *outFile, bool verbose)
{
	double magB = parameter.magB;
	double magV = parameter.magV;
	double magR = parameter.magR;
	double star_fwhm_x = parameter.star_fwhm_x;
	double star_fwhm_y = parameter.star_fwhm_y;
	std::vector<pixel_coordinates> difference = (parameter.centroid_coordinates) - (parameter.input_coordinates);

	std::vector<double> diffX(difference.size()), diffY(difference.size());
	std::transform(
		difference.begin(), difference.end(), diffX.begin(),
		[](pixel_coordinates const &ms) {
			return ms.x;
		});
	std::transform(
		difference.begin(), difference.end(), diffY.begin(),
		[](pixel_coordinates const &ms) {
			return ms.y;
		});

	double stdX = astroUtilities::StDev(diffX);
	double stdY = astroUtilities::StDev(diffY);
	printf("\n%+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f \n", magB, magV, magR, star_fwhm_x, star_fwhm_y, stdX, stdY);

	if (outFile != NULL)
	{
		if (verbose)
		{
			for (std::size_t i = 0; i < difference.size(); i++)
			{
				fprintf(outFile, "%+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f \n", magB, magV, magR, star_fwhm_x, star_fwhm_y, parameter.input_coordinates.at(i).x, parameter.input_coordinates.at(i).y, parameter.centroid_coordinates.at(i).x, parameter.centroid_coordinates.at(i).y);
			}
		}
		else
		{
			fprintf(outFile, "%+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f; %+7.4f \n", magB, magV, magR, star_fwhm_x, star_fwhm_y, stdX, stdY);
		}
	}
}

/**TODO: rewrite doc
 * Run Monte Carlo simulation for star at a given position within a pixel with a given magnitude, running for n times
 * 
 * @brief Run Monte Carlo simulation
 * @param magB B-magnitude of the star to be tested
 * @param magV V-magnitude of the star to be tested 
 * @return averageError Mean of the overall centroid recovery errors over all the iterations tested
 */
void MonteCarlo::run(std::vector<double> magB, std::vector<double> magV, std::vector<double> magR, std::vector<double> star_fwhm_x, std::vector<double> star_fwhm_y, std::vector<pixel_coordinates> coordinates, bool verbose)
{
	std::vector<FrameParameters> params_v_in = parametersVector(magB, magV, magR, star_fwhm_x, star_fwhm_y, coordinates);
	std::vector<FrameParameters> params_v_out = params_v_in;

	printf("magG; magV; magR; start_fwhm_x; star_fwhm_y; std_X; std_Y \n");

	std::vector<std::thread> v(std::thread::hardware_concurrency());

	for (unsigned i = 0; i < v.size(); ++i)
	{
		v[i] = std::thread(&MonteCarlo::runThread, this, std::ref(params_v_in), std::ref(params_v_out));
	}
	std::for_each(v.begin(), v.end(), [](std::thread &t) { t.join(); });

	//TODO: implement multithreading
	printf("\n");

	saveToFile(params_v_out, verbose);
}

void MonteCarlo::runThread(std::vector<FrameParameters> &params_v_in, std::vector<FrameParameters> &params_v_out)
{
	FrameParameters param;
	std::size_t n;

	while (true)
	{
		std::unique_lock<std::mutex> guard(params_mutex);
		std::unique_ptr<Frame> frame;

		if (params_v_in.size() > 0)
		{
			param = params_v_in.back();
			n = params_v_in.size() - 1;
			params_v_in.pop_back();

			frame = std::make_unique<Frame>(tel, expTime);
		}
		else
		{
			break;
		}
		guard.unlock();

		std::vector<double>
			magnitudes = {param.magB, param.magV, param.magR};

		for (std::vector<double>::size_type i = 0; i < param.input_coordinates.size(); i++)
		{
			//params_mutex.lock();
			printf(".");
			//params_mutex.unlock();
			pixel_coordinates center = param.input_coordinates.at(i);

			frame->reset();
			frame->addSource(center.x, center.y, param.star_fwhm_x, param.star_fwhm_y, magnitudes);
			frame->generateFrame(true);

			std::unique_ptr<FrameProcessor> fprocessor = std::make_unique<FrameProcessor>(frame->get());

			//TODO: optimize / remove hard coded values
			pixel_coordinates centroid = fprocessor->multiple_guess_momentum(30, 4, 2);
			param.centroid_coordinates.at(i) = centroid;
		}

		guard.lock();
		printFrameParametersVector(param);

		//write out the result
		params_v_out.at(n) = param;
		guard.unlock();
	}

	std::unique_lock<std::mutex> guard(params_mutex);
	printf("\n Thread done \n");
	guard.unlock();
}

void MonteCarlo::saveToFile(std::vector<FrameParameters> &parameters, bool verbose)
{

	FILE *outFile = fopen(outFileName.c_str(), "w");
	if (outFile == NULL)
	{
		std::cout << outFileName << " could not be opened for editing. "
				  << "Is it already open by another program or is it read-only?\n";
	}
	else
	{
		fprintf(outFile, "Telescope setup name: %s Exposure Time: %2.4f s\n", tel.NAME.c_str(), expTime);

#ifdef PRINT_INFO
		printf("Telescope setup name: %s Exposure Time: %2.4f s\n", tel.NAME.c_str(), expTime);
#endif

		if (verbose)
		{
			fprintf(outFile, "magB; magV; magR; fwhm_x; fwhm_y; in_x; in_y; centroid_x; centroid_y  \n");
#ifdef PRINT_INFO
			printf("magB; magV; magR; fwhm_x; fwhm_y; in_x; in_y; centroid_x; centroid_y \n");
#endif
			for (std::size_t n = 0; n < parameters.size(); n++)
			{
				printFrameParametersVector(parameters.at(n), outFile, true);
			}
		}
		else
		{
			fprintf(outFile, "magB; magV; magR; fwhm_x; fwhm_y; stdev_x; stdev_y \n");
#ifdef PRINT_INFO
			printf("magB; magV; magR; fwhm_x; fwhm_y; stdev_x; stdev_y;  \n");
#endif
			for (std::size_t n = 0; n < parameters.size(); n++)
			{
				printFrameParametersVector(parameters.at(n), outFile);
			}
		}
		fclose(outFile);
	}
}