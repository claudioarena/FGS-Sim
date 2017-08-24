<!--- Tip: Use python-pip/grip to preview Markdown documents in the same form as Github formats them. -->
# FGS-Sim
## Centroid determination simulation

Feiyu Fang

Twinkle FGS-Sim summer project, July 2017: Finding the centroid of a defocussed star to sub-pixel accuracy

## Changelog
Version number, Date in ISO 8601 format, Notes on changes

- 1.0. 2017-07-12, Initial version. 
- 1.1. 2017-07-16, Fixed x-axis incorrect determination in findCentroid method in Test.cpp
- 1.2. 2017-07-17, Changed binData method in Test.cpp to use int instead of float, representing quantised photon counts
- 1.3. 2017-07-18, Added option for using Poission noise, using an inputted SNR and sigma. 
- 2.0.0. 2017-07-20. 
    - Moved the main method of Main.cpp into void run() in Test.cpp to plot graphs of multiple results with loops in Main.cpp. 
    - Added capability in Main to create a CSV of results out of a loop for graphs
- 2.0.1-noise-testing. 2017-07-26.
    - Many bug fixes and performance improvements. 
    - Changed the noise addition to Poisson noise as X~Po(sqrt(N)). 
	- Added integration time and telescope aperture as parameters; both currently set to 1 until the photon-magnitude relation is added. 
    - Started Git sync for version control
    - Created Doxygen documentation and experimental Makefile
    - Tested whether it makes a difference if Poisson noise is calculated before binning into pixels (and then combining as RMS) or after binning.   
	  Result: 
      ![The results are different](http://i.imgur.com/AN4TjMR.png "Results")  
      The input Gaussian was kept the same by using the same seed for generating Poisson random variables both before and after passing the data through the binData function.  
      The resulting noise is different. 
    - Changed relevant functions in Test class from non-static to static to allow the above testing. 
	- Uniform distribution was added to vary the starting centre point of a simulated star. 
	- The input and output coordinates have been changed to refer to pixel coordinates instead of the arbitrary 0-1-2 original coordinate system, although the Gauss2d class
	still generates a Gaussian with mean 1 for now. 
- 2.1.0 2017-07-28. 
    - Rewrote Gauss2d class to generate a 2d array of integers to allow simulation of the defocussing of N photons into a Gaussian. Also resulted in much more efficient code, with
	the simulation running ~4x faster with excellent accuracy. 
	- Used Claudio's Google Sheet to convert stellar magnitudes into photons s^-1 m^-2 to have a stellar magnitude input in Main.cpp, resulting in N photons defocussed by Gauss2d.
	- Changed the Poisson noise method in the Test class to N - (X * time * area), where X~Po(sqrt(N)). This allows photon noise to be either positive or negative. 
	- Fixed the input of the star coordinates to be in terms of the pixel index. 
	- Bug fixes and performance and flexibility improvements. 
- 2.1.1 2017-08-01
	- Fixed addition of Poisson noise, where the number of photons recorded was added to the output matrix twice
	- Added parameters for various kinds of noise and inefficiencies:
		- Emissivity, where the ideal case is e=1
		- Quantum efficiency, where ideal case is QE=0
	- Added parameters for other noise from the [E2V CCD230-42 datasheet](https://www.e2v.com/resources/account/download-datasheet/3828 "CCD datasheet"): 
		- Temperature-dependent dark current, calculated from a temperature input. References: 
			- Dyer, "A phenomenological model for the Meyer-Neldel rule", 
			[J. Phys. C: Solid State Phys. 19 (1986) pp. 5655-5664.](http://iopscience.iop.org/article/10.1088/0022-3719/19/28/016)
			- Widenhorn et al, "Temperature dependence of dark current in a CCD", [Proc. SPIE 4669, Sensors and Camera Systems for Scientific, Industrial, and Digital Photography 
			Applications III, 193 (April 26, 2002)](http://dx.doi.org/10.1117/12.463446)
		- Readout noise, distributed according to Poisson statistics
	- Plotted the first real results. 
		- At 273K with noise but with ideal emissivity and quantum efficiency: 
		![Levels out at sigma ~40 for magnitude < 13](http://i.imgur.com/VHxvK8e.png "Result with noise")
		- Ideal Gaussian input, with no noise of any kind: 
		![Accurate for all except magnitude 15](http://i.imgur.com/fTqxLlr.png "Result without noise")
- 2.1.2 2017-08-06.
	- Looking at the error dependent on the position of the centre within the central pixel, it seems like this dependence is greater for smaller sigma, as shown below for 
	10k x 10k s-pixels binned into 100x100 pixels. 
- 2.1.3 2017-08-24
	- Notice that the E2V CCD230-42 is rated for operating temperature 153-323K. Detector to be at 72K. 
	- Added ADU parameter, set as ideal 1 electron per count for now. 
	- Updated some of the placeholder 0 and 1 variables in main() to the actual values:
		- Detector temperature: 72K; Reference: White book p67.
		- Aperture minimum 45cm; p15 and mirror M1 diameter 45cm p18. 
		- Quantum efficiency at -25C of back-illuminated E2V CCD230-42: ~0.8 with basic broadband coating at visual wavelengths. 
		- Readout noise of 230-42: 8 e- RMS at -25C at 750 kHz. 
		- Emissivity [seems to be 1](http://www.ctio.noao.edu/pipermail/ccd-world/2015/001366.html). 
	- Created block diagram of the modular code: 
	![Block diagram of the code](http://i.imgur.com/VbaaLUO.png "Flowchart of the modular code")
	- Implemented output of parameters and of photons simulated and detected. 
