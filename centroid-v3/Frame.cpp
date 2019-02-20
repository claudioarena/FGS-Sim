/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Frame.cpp
 * @brief Header file for Frame class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-01-20
 */

#include <iostream>
#include <fstream>

#include "Frame.hpp"
#include "Config/parameters.h"
#include "Test.hpp"
#include <random>
#include <chrono>

#define DEBUG

/**
 * Constructs a Frame object to generate image data arrays. 
 *
 * @param magB Magnitude of target star, in B bandpass
 * @param magV Magnitude of target star, in V bandpass
 * @param magR Magnitude of target star, in R bandpass
 * @param expTime Time of single frame, in seconds
 */

//PSF Constructor
Frame::Frame(double expTime, unsigned int width, unsigned int height)
{
    t = expTime;
    h = height;
    w = width;

    //fr.reserve(width, height);
    fr.resize(w, h);
}

Frame::~Frame()
{
}

unsigned int &Frame::operator()(unsigned int x, unsigned int y)
{
    return fr(x, y);
}
const unsigned int &Frame::operator()(unsigned int x, unsigned int y) const
{
    return fr(x, y);
}

/**
 * Add source to a Frame object to generate image data arrays. Source can be either a Guassian or a PSF, depending on what is defined in the parameters file.
 *
 * @param cx X-coordinate for source centre
 * @param cy Y-coordinate for source centre
 * @param wx source size in x direction. For gaussina source, this is the standard deviation of the Gaussian in the x direction.
 * @param wx source size in y direction. For gaussina source, this is the standard deviation of the Gaussian in the y direction.
 */
void Frame::addSource(double cx, double cy, double wx, double wy, double magB, double magV, double magR)
{
    std::vector<double> mags{magB, magV, magR};
    std::vector<struct filter> fltrs{B_filter, V_filter, R_filter};
    double telescopeArea = M_PI * pow((TEL_DIAM / 1000.0) / 2.0, 2);

    expected_photons = Test::photonsInAllBands(mags, fltrs) * telescopeArea * (1 - obstruction_area) * mirrorEfficiency * CCD_EFFICIENCY * t;
    float detectedADU = expected_photons / GAIN;

#ifdef DEBUG
    std::cout << "Average n. of photons detected: " << expected_photons << std::endl;
#endif

    //Assign pointer to the poisson distribution (for total photon counts) and seed it
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    src->photon_n_generator.seed(seed);
    src->photons = std::poisson_distribution<ulong_t>(expected_photons);

    //assign source distribution array
    src->source_prob_matrix.resize(w, h);

#if SOURCE_TYPE == GAUSSIAN
    //Normalize prob to 100
    const double A = 100 / (2 * M_PI * wx * wy);
    for (unsigned int y = 0; y < h; y++)
    {
        for (unsigned int x = 0; x < w; x++)
        {
            double xTerm = pow((x - cx), 2) / (2 * pow(wx, 2));
            double yTerm = pow((y - cy), 2) / (2 * pow(wy, 2));
            double sample_prob = (A * exp(-(xTerm + yTerm)));
            src->source_prob_matrix(x, y) = sample_prob;
        }
    }

#endif
    //add a value outside the array to the distribution func. This takes cares of photons falling outside the array.
    double prob_photon_outside_frame = 100 - src->source_prob_matrix.total();
    src->source_prob_matrix[src->source_prob_matrix.extraPixPosition()] = prob_photon_outside_frame;

    //Now assign it the vector to a distribution, and inizialize the distribution
    std::discrete_distribution<uint32_t> distribution(src->source_prob_matrix.begin(), src->source_prob_matrix.end());
    src->source_distribution = distribution;
}

void Frame::generateFrame()
{
    //Calculate tot photons received from source in this frame
    ulong_t number = src->frame_photons();
#ifdef DEBUG
    std::cout << "N. Photons in this frame: " << number << std::endl;
#endif

    //Assign pointer to the poisson distribution (for source distribution) and seed it
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    src->distribution_generator.seed(seed);

    //Distribute photons
    //TODO: Speed this up; limit based on saturation; saturation flag; if many photons, consider distribution more than one at a time.
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++)
    {
        src->photon_position();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "photo1 took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
              << " milliseconds\n";

    auto t5 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++)
    {
        fr[1205] = 5;
    }
    auto t6 = std::chrono::high_resolution_clock::now();
    std::cout << "assign took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count()
              << " milliseconds\n";

    auto t7 = std::chrono::high_resolution_clock::now();

    while (number > 0)
    {
        fr[src->photon_position()]++;
        number--;
    }

    auto t8 = std::chrono::high_resolution_clock::now();
    std::cout << "photons took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t8 - t7).count()
              << " milliseconds\n";

#ifdef DEBUG
    std::cout << "Total photons in frame array: " << std::accumulate(fr.begin(), fr.end(), 0.0) << std::endl;
    std::cout << "Total photons outside frame: " << fr[fr.extraPixPosition()] << std::endl;
#endif

    // if (pixelVal < FGS_MAX_ADU)
    // {
    //     src->source_distribution(x, y) = pixelVal;
    // }
    // else
    // {
    //     src->source_distribution(x, y) = FGS_MAX_ADU;
    // }
}

void Frame::saveToFile(std::string filename)
{
    std::ofstream file(filename.c_str(), std::ios::out);

    if (file.fail())
    {
        std::cout << filename << " could not be opened for editing. "
                  << "Is it already open by another program or is it read-only?\n";
    }
    else
    {
        file << w << ";" << h << std::endl;
        // Write each row and column of Pixels into the image file -- we write
        // from bottom left corner, going right and up

        for (int y = 0; y < h; y++)
        {
            uint16_t pixVal = (uint16_t)fr(0, y);
            file << pixVal;
            for (int x = 1; x < w; x++)
            {
                uint16_t pixVal = (uint16_t)fr(x, y);
                file << "," << pixVal;
            }
            file << std::endl;
        }

        file.close();
    }
}

void Frame::saveToBitmap(std::string filename)
{
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);

    if (file.fail())
    {
        std::cout << filename << " could not be opened for editing. "
                  << "Is it already open by another program or is it read-only?\n";
    }
    else
    {
        // Write all the header information that the BMP file format requires.
        bmpfile_magic magic;
        magic.magic[0] = 'B';
        magic.magic[1] = 'M';
        int bits_per_pixels = 24;
        int bytes_per_pixels = 3;
        file.write((char *)(&magic), sizeof(magic));
        bmpfile_header header = {0};
        header.bmp_offset = sizeof(bmpfile_magic) + sizeof(bmpfile_header) + sizeof(bmpfile_dib_info);
        //3 bytes per pixels, plus bytes needed for packing.
        uint32_t packingSize = ((w * bytes_per_pixels) % 4) * h;
        uint32_t dataSize = w * h * bytes_per_pixels;
        header.file_size = header.bmp_offset + dataSize + packingSize;
        file.write((char *)(&header), sizeof(header));
        bmpfile_dib_info dib_info = {0};
        dib_info.header_size = sizeof(bmpfile_dib_info);
        dib_info.width = w;
        dib_info.height = h;
        dib_info.num_planes = 1;
        dib_info.bits_per_pixel = bits_per_pixels;
        dib_info.compression = 0;
        dib_info.bmp_byte_size = 0;
        dib_info.hres = 2835;
        dib_info.vres = 2835;
        dib_info.num_colors = 0;
        dib_info.num_important_colors = 0;
        file.write((char *)(&dib_info), sizeof(dib_info));

        // Write each row and column of Pixels into the image file -- we write
        // from bottom left corner, going right and up
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
#if FGS_BITS == 8
                uchar_t pixVal = (uchar_t)fr(x, y);
#else
                uchar_t pixVal = (uchar_t)fr(x, y);
                if (pixVal > 255)
                {
                    pixVal = 255;
                }
#endif
                file.put(pixVal);
                file.put(pixVal);
                file.put(pixVal);
            }

            // Rows are padded so that they're always a multiple of 4
            // bytes. This line skips the padding at the end of each row.
            for (int i = 0; i < ((w * bytes_per_pixels) % 4); i++)
            {
                file.put(0);
            }
        }

        file.close();
    }
}

void Frame::setAll(unsigned int value)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            (*this)(j, i) = value;
        }
    }
}

void Frame::Print()
{
    unsigned int w = fr.width();
    unsigned int h = fr.height();
    std::cout << "Array Width: " << w << std::endl;
    std::cout << "Array Heigth : " << h << std::endl;

    for (int y = h - 1; y >= 0; y--)
    {
        for (int x = 0; x < w; x++)
        {
            std::cout << fr(x, y) << "\t";
        }
        std::cout << std::endl;
    }
}