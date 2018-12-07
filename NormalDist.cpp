// Normal distribution
#include <iostream>
#include <string>
#include <random>

using namespace std;

// Cumulative normal distribution function with X~N(0,1); F=P(X<=value)
double normalCDF(double value)
{
	return 0.5 * erfc(-value * M_SQRT1_2);
}

int main()
{

	const int nrolls = 10000;
	const int nstars = 100;

	default_random_engine generator;
	normal_distribution<double> distribution(5., 2.);

	int p[10] = {};

	for (int i = 0; i < nrolls; i++)
	{
		double number = distribution(generator);
		if ((number >= 0.) && (number < 10.))
			++p[int(number)];
	}

	cout << "normal_distribution(5.0, 2.0): " << endl;

	for (int i = 0; i < 10; i++)
	{
		cout << i << '-' << i + 1 << ": ";
		cout << string(p[i] * nstars / nrolls, '*') << endl;
	}

	cout << normalCDF(0.47) << endl;
	return 0;
}
