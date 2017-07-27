<!--- Usi python-pip/grip to preview Markdown documents in the same form as Github formats them. -->
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

