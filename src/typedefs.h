#pragma once

#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef unsigned char uchar_t;
typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef signed short int int16_t;
typedef unsigned long ulong_t;

struct pixel_coordinates
{
    double x, y;
};

inline pixel_coordinates operator-(const pixel_coordinates &lhs, const pixel_coordinates &rhs)
{
    pixel_coordinates result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}

inline std::vector<pixel_coordinates> operator-(const std::vector<pixel_coordinates> &lhs, const std::vector<pixel_coordinates> &rhs)
{
    pixel_coordinates result;

    if (lhs.size() != rhs.size())
    {
        throw "Error! Trying to subtract arrays of pixel coordinates with different sizes";
    }

    std::vector<pixel_coordinates> coordinates(lhs.size(), result);

    for (std::vector<pixel_coordinates>::size_type i = 0; i != lhs.size(); i++)
    {
        coordinates.at(i).x = lhs.at(i).x - rhs.at(i).x;
        coordinates.at(i).y = lhs.at(i).y - rhs.at(i).y;
    }

    return coordinates;
}