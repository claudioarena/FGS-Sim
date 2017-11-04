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
