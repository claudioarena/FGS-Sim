#include <chrono>
#include <cmath>
#include <random>

#include "Brownian.hpp"

using namespace std;

/**
 * Constructor to create a new Brownian object, generating increments in x and y to represent Brownian motion
 * with a given bias distance, bias angle, Brownian RMS and PSF type. 
 * @brief Constructs a new Brownian object
 *
 * @param dist Bias distance for the star movement /arcsec 
 * @param angle Bias angle for star movement /deg, with zero in the x-direction, angles defined anti-clockwise
 * @param rms RMS of Brownian motion distance /arcsec
 * @param type Whether the data source is a Huygens PSF. True for Huygens, false for FFT.
 */
Brownian::Brownian(float dist, int angle, float rms, bool type) {
	biasDistance = dist;
	biasAngle = angle;
	brownianRMS = rms;
	typeHuygens = type;
}
Brownian::~Brownian() {}

/**
 * Simulate a star moving in the field of view of the camera. This changes the position of the star with a 
 * Brownian movement and with a movement bias. 
 * @brief Generate star movement increments in x and y directions
 */
void Brownian::generate() {

	// Seed the generation of uniform-distributed random variable with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);

	float simPerDegree = -1; // Scale arcseconds into simels
	if (typeHuygens == true) simPerDegree = 5.33;
	else simPerDegree = 170.67;
	uniform_int_distribution<int> uniformAngle(1, 180); // Uniform distribution of Brownian motion angle
	normal_distribution<double> brownianDistance(0, brownianRMS * simPerDegree); // Normal distribution mean at 0, RMS 0.1", 1" or 10".
	distance = brownianDistance(generator);
	angle = uniformAngle(generator) * M_PI / 180.;
	brownianDx = distance * cos(angle); // Update the centre location after Brownian motion
	brownianDy = distance * sin(angle);

	biasDistance *= simPerDegree; // For a 6"x6" FOV and 1024x1024 simels, each degree is 170.67 simels
	brownianDx += biasDistance * cos(biasAngle * M_PI / 180.); // Update the centre location after star movement bias
	brownianDy += biasDistance * sin(biasAngle * M_PI / 180.);
}

/**
 * Resets the Brownian offsets in x and y, while maintaining the bias parameters. Function takes no parameters.
 * @brief Resets the Brownian offsets
 */
void Brownian::reset() {
	brownianDx = 0;
	brownianDy = 0;
}
