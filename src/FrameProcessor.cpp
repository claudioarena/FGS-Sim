
/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file FrameProcessor.cpp
 * @brief Header file for FrameProcessor class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-03-15
 */

#include <iostream>
#include <random>
#include <chrono>
#include "astroUtilities.hpp"
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

const pixel_coordinates FrameProcessor::momentum(const Grid<uint32_t> *fr, uint16_t threshold)
{
    pixel_coordinates centr;

    uint64_t totalWeight = total(fr, threshold);

    // Calculate x momentum
    std::vector<uint64_t> horizontalSum = sumVertical(fr, threshold); // Sum the binned data vertically
    uint64_t sum = 0;
    for (uint16_t pixelIndex = 0; pixelIndex < fr->height(); pixelIndex++)
    { // Calculate weighted mean of vertically summed data
        sum += (horizontalSum[pixelIndex] * (pixelIndex));
    }
    centr.y = (static_cast<double>(sum) / totalWeight);

    // Calculate vertical centre
    std::vector<uint64_t> verticalSum = sumHorizontal(fr, threshold); // Sum the binned data vertically
    sum = 0;
    for (uint16_t pixelIndex = 0; pixelIndex < fr->width(); pixelIndex++)
    { // Calculate weighted mean of vertically summed data
        sum += (verticalSum[pixelIndex] * (pixelIndex));
    }
    centr.x = (static_cast<double>(sum) / totalWeight);

    return centr;
};

const pixel_coordinates FrameProcessor::momentum(uint16_t threshold) const
{
    return momentum(frame, threshold);
}

uint64_t FrameProcessor::total(uint16_t threshold) const
{
    return total(frame, threshold);
}

uint64_t FrameProcessor::total(const Grid<uint32_t> *fr, uint16_t threshold)
{
    if (threshold == 0)
    {
        return std::accumulate(fr->begin(), fr->end(), 0);
    }
    else
    {
        uint64_t total = 0;
        for (uint16_t x = 0; x < fr->width(); x++)
        {
            for (uint16_t y = 0; y < fr->height(); y++)
            {
                uint32_t value = fr->operator()(x, y);
                if (value >= threshold)
                {
                    total += value;
                }
            }
        }
        return total;
    }
}

const std::vector<uint64_t> FrameProcessor::sumVertical(uint16_t threshold) const
{
    return sumVertical(frame, 0, frame->height() - 1, threshold);
}

const std::vector<uint64_t> FrameProcessor::sumVertical(uint16_t initialPos, uint16_t finalPos, uint16_t threshold) const
{
    return sumVertical(frame, initialPos, finalPos, threshold);
}

const std::vector<uint64_t> FrameProcessor::sumVertical(const Grid<uint32_t> *fr, uint16_t threshold)
{
    return sumVertical(fr, 0, fr->height() - 1, threshold);
}

const std::vector<uint64_t> FrameProcessor::sumVertical(const Grid<uint32_t> *fr, uint16_t initialPos, uint16_t finalPos, uint16_t threshold)
{
    std::vector<uint64_t> verticalVect;
    verticalVect.resize(finalPos - initialPos + 1);

    for (uint16_t y = initialPos; y <= finalPos; y++)
    {
        uint64_t sumVal = 0;
        for (uint16_t x = 0; x < fr->width(); x++)
        {
            uint32_t value = fr->operator()(x, y);
            if (value >= threshold)
            {
                sumVal += value;
            }
        }
        verticalVect[y - initialPos] = sumVal;
    }

    return verticalVect;
}

const std::vector<uint64_t> FrameProcessor::sumHorizontal(uint16_t threshold) const
{
    return sumHorizontal(frame, 0, frame->width() - 1, threshold);
}

const std::vector<uint64_t> FrameProcessor::sumHorizontal(uint16_t initialPos, uint16_t finalPos, uint16_t threshold) const
{
    return sumHorizontal(frame, initialPos, finalPos, threshold);
}

const std::vector<uint64_t> FrameProcessor::sumHorizontal(const Grid<uint32_t> *fr, uint16_t threshold)
{
    return sumHorizontal(fr, 0, fr->width() - 1, threshold);
}

const std::vector<uint64_t> FrameProcessor::FrameProcessor::sumHorizontal(const Grid<uint32_t> *fr, uint16_t initialPos, uint16_t finalPos, uint16_t threshold)
{
    std::vector<uint64_t> horizontalVect;
    horizontalVect.resize(finalPos - initialPos + 1);

    for (uint16_t x = initialPos; x <= finalPos; x++)
    {
        uint64_t sumVal = 0;
        for (uint16_t y = 0; y < fr->height(); y++)
        {
            uint32_t value = fr->operator()(x, y);
            if (value >= threshold)
            {
                sumVal += value;
            }
        }
        horizontalVect[x - initialPos] = sumVal;
    }

    return horizontalVect;
}

const pixel_coordinates FrameProcessor::initial_guess_momentum(const Grid<uint32_t> *fr, uint16_t sigma_threshold, uint8_t background_method)
{
    double background = backgroundLevel(fr, background_method);
    //TODO: change with proper stDev
    double stDev = sqrt(background);
    uint16_t threshold = round(background + (sigma_threshold * stDev));
    pixel_coordinates guess = momentum(fr, threshold);
    return guess;
}

const pixel_coordinates FrameProcessor::initial_guess_momentum(uint16_t sigma_threshold, uint8_t background_method) const
{
    return initial_guess_momentum(frame, sigma_threshold, background_method);
}

//Main method to find centroid, from whole frame to accurate guess
const pixel_coordinates FrameProcessor::multiple_guess_momentum(const Grid<uint32_t> *fr, uint16_t minWindowSize, uint16_t sigma_threshold, uint16_t sigma_threshold_final)
{
    Grid<uint32_t> subframe = (*fr);

    uint16_t newWidth = subframe.width();
    uint16_t newHeight = subframe.height();
    uint16_t offsetX = 0, offsetY = 0;
    pixel_coordinates guess;

    //find center and keep halving the size
    while (newWidth >= (minWindowSize * 2) && newHeight >= (minWindowSize * 2))
    {
        uint8_t method = Random_Global;
        if ((newWidth <= minWindowSize * 6) || (newHeight <= minWindowSize * 6))
        {
            method = Border;
        }

        guess = initial_guess_momentum(&subframe, sigma_threshold, method);
        newWidth = subframe.width() / 2.0;
        newHeight = subframe.height() / 2.0;

        subframe = subframe.subGrid(guess.x, guess.y, newWidth, newHeight, &offsetX, &offsetY);
    }

    pixel_coordinates result_centroid = initial_guess_momentum(&subframe, sigma_threshold_final, Border);
    result_centroid.x += offsetX;
    result_centroid.y += offsetY;

    result_centroid = fine_momentum(fr, result_centroid.x, result_centroid.y, minWindowSize, sigma_threshold_final);

    return result_centroid;
}

const pixel_coordinates FrameProcessor::multiple_guess_momentum(uint16_t minWindowSize, uint16_t sigma_threshold, uint16_t sigma_threshold_final) const
{
    return multiple_guess_momentum(frame, minWindowSize, sigma_threshold, sigma_threshold_final);
}

const pixel_coordinates FrameProcessor::fine_momentum(double guessX, double guessY, uint16_t windowSize, uint16_t sigma_threshold) const
{
    return fine_momentum(frame, guessX, guessY, windowSize, sigma_threshold);
}

const pixel_coordinates FrameProcessor::fine_momentum(const Grid<uint32_t> *fr, double guessX, double guessY, uint16_t windowSize, uint16_t sigma_threshold)
{
    Grid<uint32_t> grid = *fr;
    double diffX = 100, diffY = 100;
    pixel_coordinates subFrameCenter = astroUtilities::frameCenter(windowSize, windowSize);
    uint16_t maxRepetitions = 15, nRuns = 0;

    //find center and keep halving the size
    while (((abs(diffX) >= 1) || (abs(diffY) >= 1)) && (nRuns <= maxRepetitions))
    {
        uint16_t offsetX = 0, offsetY = 0;
        Grid<uint32_t> subframe = grid.subGrid(round(guessX), round(guessY), windowSize, windowSize, &offsetX, &offsetY);

        pixel_coordinates guess = initial_guess_momentum(&subframe, sigma_threshold, Border);
        diffX = subFrameCenter.x - guess.x;
        diffY = subFrameCenter.y - guess.y;
        guessX = guess.x + offsetX;
        guessY = guess.y + offsetY;

        nRuns++;
    }

    pixel_coordinates result = {guessX, guessY};
    return result;
}

//TODO: tests?
//TODO: add st deviation; normal distro with no repetiotions

const double FrameProcessor::backgroundLevel(const Grid<uint32_t> *fr, uint8_t method)
{

    uint64_t backgroundTotal = 0;
    double backgroundAverage = 0;

    if (method == FrameProcessor::Random_Global)
    {
        uint32_t nPixels = (fr->height() * fr->width());
        //Generate random frame positions to sample
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, nPixels - 1);

        //use one tenth of the pixels to estimate the background
        uint32_t selectedPixelsN = (uint32_t)((double)nPixels * 0.1);

        for (int n = 0; n < selectedPixelsN; ++n)
        {
            uint32_t pos = dis(gen);
            backgroundTotal += fr->operator[](pos);
        }
        backgroundAverage = backgroundTotal / (double)selectedPixelsN;
    }
    else if (method == FrameProcessor::Border)
    {
        uint16_t w = fr->width();
        uint16_t h = fr->height();
        uint32_t nPixels = 2 * w + 2 * (h - 2);
        for (uint16_t x = 0; x < w; x++)
        {
            backgroundTotal += fr->operator()(x, 0);
            backgroundTotal += fr->operator()(x, h - 1);
        }

        for (uint16_t y = 1; y < h - 1; y++)
        {
            backgroundTotal += fr->operator()(0, y);
            backgroundTotal += fr->operator()(w - 1, y);
        }

        backgroundAverage = backgroundTotal / (double)nPixels;
    }

    return backgroundAverage;
}

const double FrameProcessor::backgroundLevel(uint8_t method) const
{
    return backgroundLevel(frame, method);
}
