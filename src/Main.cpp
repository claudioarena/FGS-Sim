/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * Purpose: Run a Monte Carlo simulation to take a 2d PSF from Zemax, add noise, bin it into "pixels" and find
 * the centroid of the binned data.
 * NOTE: A TSV saved from Zemax has MS-DOS line endings and will need to be converted to Unix format. On Unix, 
 * the dos2unix utility is confirmed as working. 
 *
 * @file Main.cpp
 * @brief Main method to run centroid recovery simulation
 * @author Feiyu Fang
 * @version 3.2.1 2018-03-21
 */
#include "parameters.h"
#include "telescopes.h"
#include "astroUtilities.hpp"
#include "typedefs.h"
#include "MonteCarlo.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

//#include "MonteCarlo.hpp"
#include "Frame.hpp"
#include "FrameProcessor.hpp"

#include <memory>
#include <thread>

//#include "unitTest.cpp"

/*
/// Run the algorithm for a given file over the given magnitudes, outputting the average errors to file
void runFromTSV(ofstream &outputFile, std::string inFileName)
{

	float xIn = 64; // Input coordinates of defined centre in terms of pixel coordinates.
	float yIn = 64;
	int xPixels = 128;
	int yPixels = 128;
	float exposureTime = 1.0; // Time /s
	float diameter = 0.45;	// Entrance pupil diameter /m. 450mm from Twinkle whitebook.
	float QE = 0.8;
	float temperature = 72;
	float emissivity = 0.3; // Proportion of input photons sent to FGS; parameter for dichroic.
	int readout = 1;
	float ADU = 1;
	float darkSignal = 0.2; // Dark signal at 293 K. Scaling of dark signal is with the equation in the E2V CCD230-42 datasheet.
	bool zodiacal = false;

	outputFile << inFileName << ','; // This line is helpful for making the output CSV make more sense when testing multiple files at once.
	// TODO: Remove hard-coded 512 in line below. The 512 factors are to scale by the 512x512 pixel sampling from Zemax.
	MonteCarlo *m = new MonteCarlo(inFileName, xIn * 512.0 / xPixels, yIn * 512.0 / yPixels, xPixels, yPixels, exposureTime, diameter, QE, temperature, emissivity, readout, ADU, darkSignal, zodiacal);
	vector<float> uncertainty;
	for (int mag = 7; mag <= 16; mag += 1)
	{
		outputFile << m->run(mag, mag, mag, 10, 1, true) << ','; // Output values for each magnitude
		uncertainty.push_back(m->uncertainty);
	}
	for (float f : uncertainty)
		outputFile << f << ','; // Output the uncertainties after the values in the same row
	delete m;
	outputFile << endl;
}
*/

// Main method. Runs tests for given TSV input files from Zemax.
/* int main()
{

	time_t startTime = time(nullptr);
	cout << '\a' << endl
		 << "Start time: " << asctime(localtime(&startTime)) << endl;
	cout << "NOTE: If out-of-bounds errors come up, make sure that the the input file has Unix format line endings. " << endl
		 << endl;

	cout << "Configuration used: " << CONFIG << endl;
	cout << "diameter: " << TEL_DIAM << " mm" << endl;
	cout << endl;

	ofstream outFile;
	outFile.open("results2.csv");
	outFile << "Field number,Magnitude 7,Magnitude 10,Magnitude 13,SD 7,SD 10,SD 13" << endl;

	//runFromTSV(outFile, "Zemax/PSF-FFT-1024.tsv");
	runFromTSV(outFile, "Zemax/Field1.tsv");
	outFile.close();

	time_t endTime = time(nullptr);
	cout << "End time: " << asctime(localtime(&endTime)) << endl;
	cout << "Duration: " << (endTime - startTime) << " s. " << endl
		 << '\a';

	return 0;
} */

int main()
{

	Telescope tel = Twinkle;
	double expTime = 1.0; //sec
	std::string filename = "centroids1.csv";

	std::vector<double> mags = astroUtilities::makeVector(8.0, 14.0, 0.5);
	std::vector<double> fwhm = astroUtilities::makeVector(1.0, 13.0, 0.5);
	pixel_coordinates coord = pixel_coordinates{400.254, 700.524};
	std::vector<pixel_coordinates> coords = std::vector<pixel_coordinates>(500, coord);
	MonteCarlo mtc(tel, expTime, filename);
	mtc.run(mags, fwhm, coords, false);

	// expTime = 1.0; //sec
	// double star_fwhm = 6.0;
	// double star_mag = 6.0;

	// std::unique_ptr<Frame> frame = std::make_unique<Frame>(tel, expTime);
	// frame->addSource(250.1457, 651.21, star_fwhm, star_fwhm, star_mag);
	// frame->generateFrame(true);
	// frame->saveToFile("data/frame.csv");

	//totalError = 0;
	//int nruns = 1;

	//Telescope telescope = Twinkle;
	//int nruns = 10;

	//for (int i = 0; i < nruns; i++)
	//{
	//std::unique_ptr<Frame> frame = std::make_unique<Frame>(telescope, expTime);
	//frame->addSource(FRAME_CX, FRAME_CY, star_fwhm, star_fwhm, star_mag);
	//frame->addSource(20, 40, star_fwhm, star_fwhm, star_mag + 2);
	//frame->addSource(500.2, 600.3, star_fwhm, star_fwhm, star_mag);

	/*
	for (int i = 0; i < 40; i++)
	{
		double randX = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / 1024));
		double randY = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / 1024));
		double randMagChange = -4 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / 8));

		frame->addSource(randX, randY, star_fwhm, star_fwhm, star_mag + randMagChange);
	}
*/

	//frame->generateFrame();
	//f->PrintSimelArray();
	//f->Print();
	//f->saveToBitmap("data.bmp");
	//frame->saveToFile("data/frame.csv");

	/*
		std::unique_ptr<FrameProcessor> fprocessor = std::make_unique<FrameProcessor>(frame->get());

		pixel_coordinates centroid = fprocessor->momentum();
		//printf("source x: %6.12f; source y: %6.12f\n", telescope.FRAME_CX, telescope.FRAME_CY);
		//printf("cent x: %6.12f; centroid y: %6.12f\n", centroid.x, centroid.y);

		double errorX = (double)500.2 - centroid.x;
		double errorY = (double)600.3 - centroid.y;
		double totError = std::sqrt(std::pow(errorX, 2) + std::pow(errorY, 2));
		printf("run n: %d error x: %+2.4f; error y: %+2.4f, tot error: %+2.4f\n", i, errorX, errorY, totError);
		totalError += errorX;
*/
	//}
	//printf("Total error on average: %f\n", totalError / nruns);

	return 0;
}