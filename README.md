<!--- Tip: Use python-pip/grip to preview Markdown documents in the same form as Github formats them. -->
# FGS-Sim
## PHASM201 MSci Project

Feiyu Fang

Twinkle FGS-Sim MSci project, October 2017

## Changelog
Version number, Date in ISO 8601 format, Notes on changes

- 1.0. 2017-07-12, Initial version in summer project. Full summer changelog in CHANGELOG-Summer.md. 
- 2.0. 2017-07-20, Changed format to be suitable for meaningful testing.
- 2.3. 2017-09-18, Final summer version. Monte Carlo simulations from a Gaussian. 
- 3.0.0 2017-11-02
	- v3. Using a PSF instead of a generated Gaussian. 
	- Wrote a preliminarily procedural Read.cpp to read PSF data from a Zemax-generated .txt file. Output is a vector<vector<float>>.
	- NB: Input file must be converted from MS-DOS to Unix encoding. 
	- Rewrote Read.cpp into a PSF class which imports data from Zemax output into a vector<vector<int>> of photons.
	- Main.cpp now works by taking in a .tsv and calculates the coordinates of the centroid. 
- 3.0.1 2017-11-15
	- Added functions to PSF class so that the coordinates of the PSF centre can be specified, for future PSF movement use.
	- This is by taking the original normalised centroid PSF and shifting its centre, replacing empty simels with 0 values.
	- Confirmed working, with the Main.cpp correctly working out the centroid location with no Zodiacal light or readout noise, but these parameters ruin things very quickly.
- 3.0.2 2017-11-23
	- Added functions to generate Brownian motion with a given bias distance and direction in the field of view. 
	- MonteCarlo class modified to iterate over this Brownian motion, giving a path of a moving star. 
	- NB: This is only while assuming that the PSF stays the same when the star moves around the FOV. 
	- Example of Brownian motion simulation: ![Simulation of Brownian motion](https://i.imgur.com/DHzxBsl.png "True centre moves around with a bias")
- 3.1.0 2017-12-13
	- Brownian motion code moved to separate class to allow star movement within a single camera exposure
- 3.2.0 2018-02-22
	- Extracted the needed Zemax PSFs for each field on 2018-02-12. 
		- 17 fields were taken, with [location of fields shown here](https://i.imgur.com/vzUAKHN.png). 
		- Fields were taken at the centre, halfway out, and on the edge of the CCD. As the CCD is 6 arcmin across, PSFs were taken at -2.9', -1.5', 0', +1.5' and +2.9'. 
		- PSF TSVs and images of these PSFs are on Dropbox. 
	- Changed run() in MonteCarlo class to output average errors, instead of outputting directly into a CSV. 
		- This allows iterations of many Zemax PSFs. 
		- Added a public member variable for standard deviation of the errors over a set of iterations, for access and output in Main
	- File handling moved from MonteCarlo to Main. 
	- Some fields perform better than others: ![Average centroid recovery error](https://i.imgur.com/OUtU1pD.png "Some fields are better than others")
	- More Zemax work done. 
		- [Lens data taken.](https://i.imgur.com/4CSUPG5.png)
		- [Diffraction test done](https://imgur.com/a/egwEA), showing that the PSFs are generated properly. 
		- Effect of defocus on PSFs found. FWHMs change significantly once the defocus coordinate break is changed by ~0.5mm. 
		- The focal plane has a certain natural defocus due to the off-axis tilt, so [the focal plane can be re-tilted](https://i.imgur.com/RUm4rD4.png) to partially cancel this. 
- 3.2.1 2018-03-21
	- Fixed dark signal bug on Test.cpp, where temperature variation did not have any effect. 
	- Made some of the Doxygen documentation clearer
