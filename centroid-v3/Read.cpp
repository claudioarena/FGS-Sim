/**
 * Procedural code to take a Zemax PSF .txt file and extract its data into a vector<vector<float>> matrix. 
 * NOTE: The input .txt file must be converted from MS-DOS to Unix encoding; dos2unix is confirmed as working.
 *
 * @author Feiyu Fang
 * @date 2017-11-02
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main() {

	cout << "Parsing a TSV" << endl;
	ifstream file("PSF-Huygens.tsv");
	string line;

	for (int i = 0; i < 21; i++) getline(file, line); // Ignore lines in header. 21 for Huygens, 18 for FFT. 
	vector< vector<float> > matrix; // Output matrix for export
	
	while (getline(file, line)) { // Read each line into a stream
		stringstream linestream(line);
		float buffer;

		vector<float> numbersList; // Initialise row to be put into the output matrix
		while (linestream >> buffer) numbersList.push_back(buffer); // Read each token into the row vector<float>
		for (unsigned i = 0; i < numbersList.size(); i++) cout << numbersList[i] << ' ';
		cout << "; Total tokens: " << numbersList.size() << endl;
		matrix.push_back(numbersList);
	}

	cout << "Total rows: " << matrix.size() << ". End. " << endl;
	return 0;
}
