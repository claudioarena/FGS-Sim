#include "Config\parameters.h"
#include "Config\telescopes.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

//#include "MonteCarlo.hpp"
#include "Frame.hpp"
#include "FrameProcessor.hpp"

#include <random>
#include <memory>

using namespace std;

class unitTests
{
  public:
    /*static int TestNumberPhotons()
    {
        //if (CONFIG != "20 cm Newtonian")
        //{
        //    throw "This test should run on 20cm config file!";
        //}

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
    static int TestAirmass()
    {
        printf("Airmass at 90 altitude: %4.2f \n\r", Test::airmass(90));
        printf("Extinction perc: %4.2f \n\r", Test::extinctionInPercentage(90));
        printf("Airmass st 5 altitude: %4.2f \n\r", Test::airmass(5));
        printf("Extinction perc: %4.2f \n\r", Test::extinctionInPercentage(5));
    }

    static int TestTwinkleMags()
    {
        if (CONFIG != "Twinkle")
        {
            throw "This test should run on Twinkle config file!";
        }

        const float expectedADU = 6178.0;
        std::vector<struct filter> fltrs{B_filter, V_filter, R_filter};
        std::vector<double> mags{14.0, 14.0, 14.0};

        float received_photons = Test::photonsInAllBands(mags, fltrs) * 1.0;
        cout << "Received photons: " << received_photons << " photons m-2" << endl;
        float telescopeArea = M_PI * pow((TEL_DIAM / 1000.0) / 2.0, 2);
        float detectedPhotons = received_photons * telescopeArea * (1 - obstruction_area) * mirrorEfficiency * CCD_EFFICIENCY; //per second
        cout << "Detected photons: " << detectedPhotons << "photons" << endl;
        cout << "Uncertainty in ADU: " << sqrt(detectedPhotons) / GAIN << endl;
        cout << "Expected ADU: " << expectedADU << endl;
    }*/
};
