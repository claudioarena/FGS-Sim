#include "parameters.h"
#include "telescopes.h"
#include "typedefs.h"
#include "Frame.hpp"
#include "FrameProcessor.hpp"
#include "astroUtilities.hpp"

#include <memory>
#include "gtest/gtest.h"

Telescope tel = Twinkle;
double expTime = 1.0; //sec
double star_fwhm = 5.0;
double star_mag = 13.0;

//TODO: Test generation of small array. Check print, reset, setall
//TODO: Check all utilities
//TODO: check utility for center of frame:
/* 	printf("center x: %f", Twinkle.FRAME_CX);
	printf("center y: %f", Twinkle.FRAME_CY);
	pixel_coordinates center = astroUtilities::frameCenter(Twinkle.FRAME_W, Twinkle.FRAME_H);
	printf("center x: %f", center.x);
	printf("center y: %f", center.y);

    	printf("Mirror efficiency: %f", Twinkle.mirrorEfficiency);
	printf("Obstruction Area: %f", Twinkle.obstruction_area);

	printf("Mirror efficiency: %f", astroUtilities::reflectionEfficiency(Twinkle.COATING_REFLECTIVITY, Twinkle.N_MIRRORS_TO_CAMERA));
	printf("Obstruction Area: %f", astroUtilities::obstructionPercentage(Twinkle.DIAMETER, Twinkle.SECONDARY_DIAMETER));
    */

std::unique_ptr<Frame> frame = std::make_unique<Frame>(tel, expTime);

TEST(Telescopes, parameters)
{
    EXPECT_FLOAT_EQ(tel.DIAMETER, 450);
}

TEST(Frame, constructor)
{
    frame = std::make_unique<Frame>(tel, expTime);
    //frame->addSource(FRAME_CX, FRAME_CY, star_fwhm, star_fwhm, star_mag);
    EXPECT_TRUE(true);
}

pixel_coordinates checkCentroid(double x, double y, bool statistical)
{
    frame->reset();

    std::unique_ptr<Frame> frame = std::make_unique<Frame>(tel, expTime);
    frame->addSource(x, y, star_fwhm, star_fwhm, star_mag);
    frame->generateFrame(statistical);

    std::unique_ptr<FrameProcessor> fprocessor = std::make_unique<FrameProcessor>(frame->get());

    uint16_t background = fprocessor->backgroundLevel(fprocessor->Random_Global);

    pixel_coordinates centroid = fprocessor->multiple_guess_momentum(30, 20);
    //double centx = centroid.x;
    //double centy = centroid.y;

    return centroid;
    //printf("source x: %6.12f; source y: %6.12f\n", x, y);
    //printf("cent x: %6.12f; centroid y: %6.12f\n", centx, centy);
}

pixel_coordinates checkCentroid(pixel_coordinates coord, bool statistical)
{
    return checkCentroid(coord.x, coord.y, statistical);
}

TEST(FrameProcessor, centroid)
{
    star_mag = 16.0;
    pixel_coordinates center = astroUtilities::frameCenter(Twinkle.FRAME_W, Twinkle.FRAME_H);
    pixel_coordinates momentum;
    uint16_t x, y;
    double maxErr = 0.001;

    x = center.x;
    y = center.y;
    momentum = checkCentroid(center, false);
    maxErr = 0.001;
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);

    x = 100.3;
    y = 300.8;
    momentum = checkCentroid(x, y, false);
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);

    x = 800.0;
    y = 600.0;
    momentum = checkCentroid(x, y, false);
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);
}

TEST(FrameProcessor, centroid_statistical)
{
    star_mag = 10.0;
    pixel_coordinates center = astroUtilities::frameCenter(Twinkle.FRAME_W, Twinkle.FRAME_H);
    pixel_coordinates momentum;
    uint16_t x, y;
    double maxErr = 0.1;

    x = center.x;
    y = center.y;
    momentum = checkCentroid(center, true);
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);

    x = 100.3;
    y = 300.8;
    momentum = checkCentroid(x, y, true);
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);

    x = 800.0;
    y = 600.0;
    momentum = checkCentroid(x, y, true);
    EXPECT_NEAR(momentum.x, x, maxErr);
    EXPECT_NEAR(momentum.y, y, maxErr);
}

TEST(astroUtilities, airMass)
{
    EXPECT_NEAR(astroUtilities::airmass(5), 10.334, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(15), 3.808, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(20), 2.900, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(30), 1.993, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(45), 1.412, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(70), 1.064, 0.001);
    EXPECT_NEAR(astroUtilities::airmass(90), 1.0, 0.0001);
}

TEST(astroUtilities, extinction)
{
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(5, 0.35), 0.0357, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(15, 0.35), 0.293, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(20, 0.35), 0.392, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(30, 0.35), 0.526, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(45, 0.35), 0.634, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(70, 0.35), 0.710, 0.001);
    EXPECT_NEAR(astroUtilities::extinctionInPercentage(90, 0.35), 0.724, 0.001);
}

TEST(astroUtilities, photonNumbers)
{
    const double expectedADU = 6178.0 * Twinkle.GAIN;
    std::vector<struct filter> fltrs{B_filter, V_filter, R_filter};
    std::vector<double> mags{14.0, 14.0, 14.0};

    double detectedPhotons = astroUtilities::meanReceivedPhotons(mags, fltrs, 1.0, Twinkle);

    EXPECT_NEAR(detectedPhotons, expectedADU, 3.0);
}

TEST(astroUtilities, average)
{
    std::vector<double> numbers{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double average = astroUtilities::Average(numbers);
    EXPECT_DOUBLE_EQ(average, 0.0);

    std::vector<int> numbers2{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double average2 = astroUtilities::Average(numbers2);
    EXPECT_DOUBLE_EQ(average2, 0);

    std::vector<int> numbers3{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    double average3 = astroUtilities::Average(numbers3);
    EXPECT_DOUBLE_EQ(average3, 8.5);

    std::vector<double> numbers4{2.5, 3.4, 4.1, 5.2, 14, 15};
    double average4 = astroUtilities::Average(numbers4);
    EXPECT_NEAR(average4, 7.366, 0.01);

    std::vector<double> numbers5{-5, 10, 50.8, 35, -80};
    double average5 = astroUtilities::Average(numbers5);
    EXPECT_NEAR(average5, 2.16, 0.01);
}

TEST(astroUtilities, StDev)
{
    std::vector<double> numbers{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double stDev = astroUtilities::StDev(numbers);
    EXPECT_DOUBLE_EQ(stDev, 0.0);

    std::vector<int> numbers2{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double stDev2 = astroUtilities::StDev(numbers2);
    EXPECT_DOUBLE_EQ(stDev2, 0);

    std::vector<int> numbers3{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    double stDev3 = astroUtilities::StDev(numbers3);
    EXPECT_NEAR(stDev3, 4.1833, 0.0001);

    std::vector<double> numbers4{2.5, 3.4, 4.1, 5.2, 14, 15};
    double stDev4 = astroUtilities::StDev(numbers4);
    EXPECT_NEAR(stDev4, 5.604522, 0.0001);

    std::vector<double> numbers5{-5, 10, 50.8, 35, -80};
    double stDev5 = astroUtilities::StDev(numbers5);
    EXPECT_NEAR(stDev5, 50.76247, 0.0001);
}

/*
        const float altitude = 38.0;
        const float expectedADU = 167274;
        const float expTime = 250.0; //seconds
        std::vector<double> mags{15.735, 15.172, 15.035};
        std::vector<struct filter> fltrs{B_filter, V_filter, R_filter};

        float received_photons = Test::photonsInAllBands(mags, fltrs) * expTime;
        cout << "Received photons: " << received_photons << endl;
        float telescopeArea = M_PI * pow((TEL_DIAM / 1000.0) / 2.0, 2);
        float detectedPhotons = received_photons * telescopeArea * (1 - obstruction_area) * mirrorEfficiency * CCD_EFFICIENCY; //per second
        cout << "Detected photons: " << detectedPhotons << endl;
        detectedPhotons = detectedPhotons * Test::extinctionInPercentage(altitude); //per secons
        cout << "Detected photons (with extinction): " << detectedPhotons << endl;
        float measuredADU = detectedPhotons / GAIN; //per second
        cout << "Detected ADU: " << measuredADU << endl;
        cout << "Uncertainty in ADU: " << sqrt(detectedPhotons) / GAIN << endl;
        cout << "Expected ADU: " << expectedADU << endl;

        detectedPhotons = received_photons * telescopeArea * (1 - obstruction_area) * mirrorEfficiency * CCD_EFFICIENCY; //per second
        float expectedPhotons = expectedADU * GAIN;
        float measureExtinction = expectedPhotons / detectedPhotons;
        float measuredExtinctionMags = -2.5 * log10(measureExtinction);

        float measuredExtinctionCoefficient = measuredExtinctionMags / Test::airmass(altitude);

        return 0;
    }

    }*/