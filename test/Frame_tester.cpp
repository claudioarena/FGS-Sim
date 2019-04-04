#include "parameters.h"
#include "telescopes.hpp"
#include "typedefs.h"
#include "Frame.hpp"
#include "FrameProcessor.hpp"

#include <memory>
#include "gtest/gtest.h"

Telescope tel = Twinkle;
std::unique_ptr<Frame> frame;

double expTime = 1.0; //sec
double star_fwhm = 5.0;
double star_mag = 13.0;

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

TEST(Frame, photonNum)
{
    //TODO: add check for photon numbers, both Twinkle and 20 cm
    //frame->
}

void checkCentroid(double x, double y, double maxErr, bool statistical)
{
    frame = std::make_unique<Frame>(tel, expTime);
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
    checkCentroid(tel.FRAME_CX, tel.FRAME_CY, 0.001, false);
    checkCentroid(100.3, 300.8, 0.001, false);
    checkCentroid(800.0, 600.0, 0.001, false);
}

TEST(FrameProcessor, centroid_statistical)
{
    star_mag = 10.0;
    checkCentroid(tel.FRAME_CX, tel.FRAME_CY, 0.01, true);
    checkCentroid(100.3, 300.8, 0.01, true);
    checkCentroid(800.0, 600.0, 0.01, true);
}