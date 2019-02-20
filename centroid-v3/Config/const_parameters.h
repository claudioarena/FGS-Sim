#ifndef CONST_PARAM
#define CONST_PARAM

const float SB_CONST = 5.67E-8; // Stefan-Boltzmann constant
const float WIEN = 2.9E-3;      // Wien's displacement constant
const float PLANCK = 6.63E-34;  // Planck's constant

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
const struct filter IntegratedVisual_filter = {550, 350, 3640};

#endif