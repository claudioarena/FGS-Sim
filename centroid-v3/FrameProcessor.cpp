
/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file FrameProcessor.cpp
 * @brief Header file for FrameProcessor class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-03-15
 */

#include <iostream>
#include "FrameProcessor.hpp"
#define DEBUG

/**
 * Constructs a FrameProcessor object to analyse image data arrays. 
 *
 * @param _frame pointer to constant grid object representing the image
 */
FrameProcessor::~FrameProcessor()
{
#ifdef DEBUG_MEMORY
    std::cout << "Destroyed Frame Processor " << std::endl;
#endif
}

const uint32_t &FrameProcessor::operator()(unsigned int x, unsigned int y) const
{
    return frame->operator()(x, y);
}

const uint64_t FrameProcessor::total() const
{
    return std::accumulate(frame->begin(), frame->end(), 0);
}

const std::vector<uint64_t> FrameProcessor::sumVertical() const
{
    return sumVertical(0, frame->height() - 1);
}

const centroid FrameProcessor::momentum() const
{
    centroid centr;

    uint64_t totalWeight = total();

    // Calculate x momentum
    std::vector<uint64_t> horizontalSum = sumVertical(); // Sum the binned data vertically
    uint64_t sum = 0;
    for (uint16_t pixelIndex = 0; pixelIndex < frame->width(); pixelIndex++)
    { // Calculate weighted mean of vertically summed data
        sum += (horizontalSum[pixelIndex] * (pixelIndex));
    }
    centr.x = (static_cast<double>(sum) / totalWeight);

    // Calculate vertical centre
    std::vector<uint64_t> verticalSum = sumHorizontal(); // Sum the binned data vertically
    sum = 0;
    for (uint16_t pixelIndex = 0; pixelIndex < frame->height(); pixelIndex++)
    { // Calculate weighted mean of vertically summed data
        sum += (verticalSum[pixelIndex] * (pixelIndex));
    }
    centr.y = (static_cast<double>(sum) / totalWeight);

    return centr;
}

const std::vector<uint64_t> FrameProcessor::sumVertical(uint16_t initialPos, uint16_t finalPos) const
{
    std::vector<uint64_t> verticalVect;
    verticalVect.resize(finalPos - initialPos + 1);

    for (uint16_t y = initialPos; y <= finalPos; y++)
    {
        uint64_t sumVal = 0;
        for (uint16_t x = 0; x < frame->width(); x++)
        {
            sumVal += frame->operator()(x, y);
        }
        verticalVect[y - initialPos] = sumVal;
    }

    return verticalVect;
}

const std::vector<uint64_t> FrameProcessor::sumHorizontal() const
{
    return sumHorizontal(0, frame->width() - 1);
}

const std::vector<uint64_t> FrameProcessor::sumHorizontal(uint16_t initialPos, uint16_t finalPos) const
{
    std::vector<uint64_t> horizontalVect;
    horizontalVect.resize(finalPos - initialPos + 1);

    for (uint16_t x = initialPos; x <= finalPos; x++)
    {
        uint64_t sumVal = 0;
        for (uint16_t y = 0; y < frame->height(); y++)
        {
            sumVal += frame->operator()(x, y);
        }
        horizontalVect[x - initialPos] = sumVal;
    }

    return horizontalVect;
}