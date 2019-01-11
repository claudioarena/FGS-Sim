/*---------------------------------------------------------
 * Integrate over a Gaussian using the Trapezium Rule 
 * Feiyu Fang 10-07-2017
 *-------------------------------------------------------*/
#include <cmath>
#include <iostream>

using namespace std;

// Calculate Gaussian value at x, with mean mu and SD sigma
float gaussDist(float x, float mu, float sigma)
{

	//cout << "Mean = " << mu << "; SD = " << sigma << endl;

	// Calculate function at x
	float out = (1 / sqrt(2 * M_PI * sigma * sigma)) * exp(-1 * pow(x - mu, 2) / (2 * sigma * sigma));
	return out;
}

// Integrate over the Gaussian using the Trapezium Rule.
// Inputs: strip width h, integral limits x0 and xn, mean m, SD s.
float trapezium(float h, float x0, float xn, float m, float s)
{

	float y0 = gaussDist(x0, m, s); // First and last strips
	float yn = gaussDist(xn, m, s);

	float internal = 0;
	for (float i = x0; i < xn; i += h)
		internal += gaussDist(i, m, s); // Sum heights of internal strips: (y1 + y2 + y3 + etc)

	return 0.5 * h * (y0 + yn + 2 * internal);
}

int main()
{

	cout << trapezium(0.01, -1, 1, 0, 1) << endl;

	return 0;
}
