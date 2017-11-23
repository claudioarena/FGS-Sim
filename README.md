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
