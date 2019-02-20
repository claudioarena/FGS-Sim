#ifndef TELESCOPE_PAR
#define TELESCOPE_PAR

#include "const_parameters.h"
#include <cmath>

#define CONFIG "Twinkle"
#define TEL_DIAM 450              //mm
#define EXTICTION_COEFFICIENT 0.0 //in space
#define N_MIRRORS_TO_CAMERA 5     //M1, M2, fold, M3, FGS_M
#define COATING_REFLECTIVITY 0.94 //Al
#define SECONDARY_DIAMETER 85.0   //mm
#define CCD_EFFICIENCY 0.65       //Average in bandpass
#define GAIN 1.0                  //e- / ADU

#define FRAME_W 1920
#define FRAME_H 1080
#define SOURCE_TYPE GAUSSIAN
//#define SOURCE_TYPE PSF
#define FRAME_CX (FRAME_W / 2.0) + 0.5
#define FRAME_CY (FRAME_H / 2.0) + 0.5
//#define FGS_BITS 8
#define FGS_BITS 16
#define FGS_MAX_ADU pow(2, FGS_BITS) - 1

static const float emiss = 0.02; // Mirror emissivity
static const struct filter FGS_filter = B_filter;

static const float obstruction_area = pow(SECONDARY_DIAMETER, 2) / pow(TEL_DIAM, 2);  // 0-100 % -> 0-1. In percentage area of primary mirror
static const float mirrorEfficiency = pow(COATING_REFLECTIVITY, N_MIRRORS_TO_CAMERA); // 0-1 coating reflecting efficiency, averaged over wavelength range

#endif