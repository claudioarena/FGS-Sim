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

void checkCentroid(double x, double y, double maxErr, bool statistical)
{
    frame->reset();

    std::unique_ptr<Frame> frame = std::make_unique<Frame>(tel, expTime);
    frame->addSource(x, y, star_fwhm, star_fwhm, star_mag);
    frame->generateFrame(statistical);

    std::unique_ptr<FrameProcessor> fprocessor = std::make_unique<FrameProcessor>(frame->get());

    centroid centroid = fprocessor->momentum();
    double centx = centroid.x;
    double centy = centroid.y;

    EXPECT_NEAR(centx, x, maxErr);
    EXPECT_NEAR(centy, y, maxErr);
    //printf("source x: %6.12f; source y: %6.12f\n", x, y);
    //printf("cent x: %6.12f; centroid y: %6.12f\n", centx, centy);
}

TEST(FrameProcessor, centroid)
{
    star_mag = 15.0;
    checkCentroid(Twinkle.FRAME_CX, Twinkle.FRAME_CY, 0.001, false);
    checkCentroid(100.3, 300.8, 0.001, false);
    checkCentroid(800.0, 600.0, 0.001, false);
}

TEST(FrameProcessor, centroid_statistical)
{
    star_mag = 10.0;
    checkCentroid(Twinkle.FRAME_CX, Twinkle.FRAME_CY, 0.01, true);
    checkCentroid(100.3, 300.8, 0.01, true);
    checkCentroid(800.0, 600.0, 0.01, true);
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
    //const float expectedADU = 6178.0;
    const float expectedADU = 6976.0;
    std::vector<struct filter> fltrs{B_filter, V_filter, R_filter};
    std::vector<double> mags{14.0, 14.0, 14.0};

    float received_photons = astroUtilities::photonsInBand(mags, fltrs) * 1.0;

    float telescopeArea = M_PI * pow((Twinkle.DIAMETER / 1000.0) / 2.0, 2);
    float detectedPhotons = received_photons * telescopeArea * (1 - Twinkle.obstruction_area) * Twinkle.mirrorEfficiency * Twinkle.mirrorEfficiency; //per second

    EXPECT_NEAR(detectedPhotons, expectedADU, 3.0);
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