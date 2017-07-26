// Normal distribution
#include <iostream>
#include <string>
#include <random>

using namespace std;

int main() {
	
	const int nrolls = 10000;
	const int nstars = 100;

	default_random_engine generator;
	normal_distribution<double> distribution(5., 2.);

	int p[10] = {};

	for (int i = 0; i < nrolls; i++) {
		double number = distribution(generator);
		if ((number >= 0.) && (number < 10.)) ++p[int(number)];
	}

	cout << "normal_distribution(5.0, 2.0): " << endl;

	for (int i = 0; i < 10; i++) {
		cout << i << '-' << i + 1 << ": ";
		cout << string(p[i] * nstars/nrolls, '*') << endl;
	}
	return 0;
}
