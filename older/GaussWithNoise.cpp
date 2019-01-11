/*-----------------------------------------------------------------
 * Generate a Gaussian with noise
 * Feiyu Fang 10-07-2017
 * 
 *---------------------------------------------------------------*/

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

// Calculate Gaussian value at x, with mean mu and SD sigma
float gaussDist(float x, float mu, float sigma)
{

	// Create Gaussian noise, seeded by system time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	normal_distribution<double> distribution(0, 1);

	double noise = distribution(generator) / 100.;
	return (1 / sqrt(2 * M_PI * sigma * sigma)) * exp(-1 * pow(x - mu, 2) / (2 * sigma * sigma)) + noise;
}

// Integrate over the Gaussian using the Trapezium Rule.
// Inputs: strip width h, integral limits x0 and xn, mean m, SD s.
float trapezium(float h, float x0, float xn, float m, float s)
{

	float y0 = gaussDist(x0, m, s); // First and last strips
	float yn = gaussDist(xn, m, s);

	float internal = 0; // Initialise var to hold sum of heights of internal strips: (y1 + y2 + y3 + etc)
	for (float i = x0; i < xn; i += h)
	{
		internal += gaussDist(i, m, s); // Sum internal heights with noise
	}
	return 0.5 * h * (y0 + yn + 2 * internal);
}

int main()
{

	//cout << trapezium(0.1, -1, 1, 0, 1) << endl; // Integral between -sigma and sigma

	int h = 10; // Number of pixels in each dimension
	float gaussianX[2 * h];

	// Generate Gaussian with noise.
	for (int i = 0; i < (2 * h); i++)
	{
		gaussianX[i] = gaussDist(float(i) / h, 1., 1.);
		cout << i << ": " << gaussianX[i] << endl;
	}

	return 0;
}
