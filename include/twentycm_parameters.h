#ifndef TELESCOPE_PAR
#define TELESCOPE_PAR

#include "const_parameters.h"
#include <cmath>

#define CONFIG "20 cm Newtonian"
#define TEL_DIAM 200 //in mm
#define EXTICTION_COEFFICIENT 0.35
#define N_MIRRORS_TO_CAMERA 2
#define COATING_REFLECTIVITY 0.94
#define SECONDARY_DIAMETER 63.0
#define CCD_EFFICIENCY 0.5 //Average, over filter bandpass
#define GAIN 0.267         // e- / ADU

static const float emiss = 0.02; // Mirror emissivity
static const struct filter FGS_filter = B_filter;

static const float obstruction_area = pow(SECONDARY_DIAMETER, 2) / pow(TEL_DIAM, 2);  // 0-100 % -> 0-1. In percentage area of primary mirror
static const float mirrorEfficiency = pow(COATING_REFLECTIVITY, N_MIRRORS_TO_CAMERA); // 0-1 coating reflecting efficiency, averaged over wavelength range

#endif