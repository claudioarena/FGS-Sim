#pragma once
#include <cmath>
#include "typedefs.h"
#include <string>
#include <vector>

//define filter properties
struct filter
{
    float centerBand;
    float bandWidth;
    float zero_point_Jy;
};
const struct filter B_filter = {440, 96.8, 4260};
const struct filter V_filter = {550, 88, 3640};
const struct filter R_filter = {640, 147.2, 3080};
//const struct filter IntegratedVisual_filter = {550, 350, 3640};

struct Telescope
{
    const std::string NAME;
    const std::string SOURCE_TYPE;

    //simulation elements per pixel, per side. So, value of 10 means 10x10 simels per pixels.
    const uint16_t SIMELS;
    const double DIAMETER;               //mm
    const double EXTINCTION_COEFFICIENT; //in space
    const double N_MIRRORS_TO_CAMERA;    //M1, M2, fold, M3, FGS_M
    const double COATING_REFLECTIVITY;   //Al
    const double SECONDARY_DIAMETER;     //mm
    const double CCD_EFFICIENCY;         //Average in bandpass
    const double GAIN;                   //e- / ADU

    const uint16_t FRAME_W;
    const uint16_t FRAME_H;
    //#define SOURCE_TYPE GAUSSIAN
    //#define SOURCE_TYPE PSF
    //#define FGS_BITS 8
    const uint16_t FGS_BITS;
    const uint32_t FGS_MAX_ADU;
    const double DARK_NOISE;    // e/s/pix at 0C
    const double READOUT_NOISE; // e/pix
    double FGS_CCD_TEMP;        // in K
    double IR_CCD_TEMP;         // in K
    const double emiss;         // Mirror emissivity
    const std::vector<filter> FGS_filter;
};

static Telescope Twinkle = {
    .NAME = "Twinkle",
    .SOURCE_TYPE = "GAUSSIAN",
    .SIMELS = 1,
    .DIAMETER = 450.0,             //mm
    .EXTINCTION_COEFFICIENT = 0.0, //in space
    .N_MIRRORS_TO_CAMERA = 5.0,    //M1, M2, fold, M3, FGS_M
    .COATING_REFLECTIVITY = 0.94,  //Al
    .SECONDARY_DIAMETER = 85.0,    //mm
    .CCD_EFFICIENCY = 0.65,        //Average in bandpass
    .GAIN = 1.0,                   //e- / ADU
    .FRAME_W = 1024,
    .FRAME_H = 1024,
    .FGS_BITS = 16,
    .FGS_MAX_ADU = (uint32_t)pow(2, Twinkle.FGS_BITS) - 1,
    .DARK_NOISE = 2.8761, // e- / pixel / sec. At 0C, derived from datasheet E2V CCD230-42. Note that this is a rough value.
    .READOUT_NOISE = 4.0, //from e2v CCD230-42 datasheet
    .FGS_CCD_TEMP = 250,  //Kelvin
    .IR_CCD_TEMP = 70,    //Kelvin
    .emiss = 0.02,        // Mirror emissivity
    .FGS_filter = {B_filter, V_filter, R_filter},
};

static Telescope TwentyCm = {
    .NAME = "Twenty Cm",
    .SOURCE_TYPE = "GAUSSIAN",
    .SIMELS = 1,
    .DIAMETER = 200.0,              //mm
    .EXTINCTION_COEFFICIENT = 0.35, //in space
    .N_MIRRORS_TO_CAMERA = 2.0,     //M1, M2, fold, M3, FGS_M
    .COATING_REFLECTIVITY = 0.94,   //Al
    .SECONDARY_DIAMETER = 63.0,     //mm
    .CCD_EFFICIENCY = 0.5,          //Average in bandpass
    .GAIN = 0.267,                  //e- / ADU
    .FRAME_W = 1280,
    .FRAME_H = 1024,
    .FGS_BITS = 16,
    .FGS_MAX_ADU = (uint32_t)pow(2, TwentyCm.FGS_BITS) - 1,
    .DARK_NOISE = 0.1,
    .READOUT_NOISE = 4,
    .FGS_CCD_TEMP = 273,
    .IR_CCD_TEMP = 273,
    //.FGS_BITS = 8,
    .emiss = 0.02, // Mirror emissivity
    .FGS_filter = {B_filter, V_filter, R_filter},
};
/*
    printf("%f\n", Twinkle.CCD_EFFICIENCY);
    printf("%f\n", Twinkle.COATING_REFLECTIVITY);
    printf("%f\n", Twinkle.emiss);
    printf("%f\n", Twinkle.EXTINCTION_COEFFICIENT);
    printf("%d\n", Twinkle.FGS_BITS);
    printf("%f\n", Twinkle.FGS_filter.bandWidth);
    printf("%f\n", Twinkle.FGS_filter.centerBand);
    printf("%f\n", Twinkle.FGS_filter.zero_point_Jy);
    printf("%f\n", Twinkle.FGS_filter);
    printf("%d\n", Twinkle.FGS_MAX_ADU);
    printf("%f\n", Twinkle.FRAME_CX);
    printf("%f\n", Twinkle.FRAME_CY);
    printf("%d\n", Twinkle.FRAME_H);
    printf("%d\n", Twinkle.FRAME_W);
    printf("%f\n", Twinkle.GAIN);
    printf("%f\n", Twinkle.mirrorEfficiency);
    printf("%f\n", Twinkle.N_MIRRORS_TO_CAMERA);
    printf("%f\n", Twinkle.obstruction_area);
    printf("%f\n", Twinkle.SECONDARY_DIAMETER);
    printf("%f\n", Twinkle.TEL_DIAM);

*/
