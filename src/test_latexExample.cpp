//First, include the header files and classes needed
#include "parameters.h"
#include "telescopes.h"
#include "astroUtilities.hpp"
#include "typedefs.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <random>

#include "Frame.hpp"
#include "FrameProcessor.hpp"
   
int main() {
//Let us simulate a Twinkle like telescope, with a 10 second exposure on a 10 mag star, at a given coordinate and fwhm.
Telescope tel = Twinkle;
double expTime = 30.0; //sec
double star_fwhm = 4.0;
double star_mag = 10.0;
pixel_coordinates coord = pixel_coordinates{300.0, 400.0};
    
// random device class instance, source of 'true' randomness for initializing random seed
std::random_device rd; 

// Mersenne twister PRNG, initialized with seed from previous random device instance
std::mt19937 gen(rd()); 
    
std::normal_distribution<float> d(14, 2); 

//Let us now construct the frame, assuming same fwhm along x and y
std::unique_ptr<Frame> frame=std::make_unique<Frame>(tel, expTime);
//frame->addSource(coord.x, coord.y, star_fwhm, star_fwhm, star_mag);
//frame->addSource(100, 200, star_fwhm, star_fwhm, star_mag);
for (int i = 0; i < 50; i++) {
    frame->addSource(rand() % 1022, rand() % 1022, star_fwhm, star_fwhm, d(gen));
}

frame->generateFrame(true);
frame->saveToFile("testing.txt");

//And let us reconstruct the centroid
//std::unique_ptr<FrameProcessor> fprocessor = std::make_unique<FrameProcessor>(frame->get());
//uint16_t minWindowSize = 30;
//uint16_t sigma_threshold = 4;
//uint16_t sigma_threshold_final = 2;
//pixel_coordinates centroid = fprocessor->multiple_guess_momentum(minWindowSize, sigma_threshold, sigma_threshold_final);
   
//And print the result
//printf("centroid x: %6.12f; centroid y: %6.12f\n", centroid.x, centroid.y);

//We can of course save and analyse the produced frame, or generate more frames and see how the centroid reconstruction varies with the randomly generated background noise.

return 0;
}