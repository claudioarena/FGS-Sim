/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Frame.cpp
 * @brief Header file for Frame class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-01-20
 * @description Creates a simulated frame. Coordinate system used: Center of frame is x.0, y.0. Edge of frame is x.5,y.5. x.999 is very close to x+1.0.
 * Trying to follow convention that top left pixel is 0,0. If reading frame on MaximDL, coordinate readout should match.
 * Note that AstroImageJ has a 1 pixel offset, and a different reference system in some modes.
 * MaximDL readout for FWHM is not accurate. ImageJ is.
 */
//TODO: reset frame so we can generate again.
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>

#include "Frame.hpp"
#include "Test.hpp"
#include "astroUtilities.hpp"

//#define DEBUG
//#define PRINT_PROB_ARRAY
//#define PRINT_SOURCE_DATA
//#define TIMING

/**
 * Constructs a Frame object to generate image data arrays. 
 *
 * @param magB Magnitude of target star, in B bandpass
 * @param magV Magnitude of target star, in V bandpass
 * @param magR Magnitude of target star, in R bandpass
 * @param expTime Time of single frame, in seconds
 */

//PSF Constructor
Frame::Frame(Telescope _tel, double _expTime)
    : tel(_tel), t(_expTime), h(_tel.FRAME_H), w(_tel.FRAME_W), hsim(h * _tel.SIMELS), wsim(w * _tel.SIMELS)
{
    fr.resize(w, h);
    simfr.resize(wsim, hsim);
    sources.reserve(10);

#ifdef DEBUG_MEMORY
    std::cout
        << "Created Frame " << std::endl;
#endif
}

Frame::~Frame()
{
#ifdef DEBUG_MEMORY
    std::cout << "Destroyed Frame " << std::endl;
#endif
}

uint32_t &Frame::operator()(uint16_t x, uint16_t y)
{
    return fr(x, y);
}
const uint32_t &Frame::operator()(uint16_t x, uint16_t y) const
{
    return fr(x, y);
}

/**
 * Add source to a Frame object to generate image data arrays. Source can be either a Guassian or a PSF, depending on what is defined in the parameters file.
 *
 * @param cx X-coordinate for source centre
 * @param cy Y-coordinate for source centre
 * @param fwhm_x source size in x direction.
 * @param fwhm_x source size in y direction.
 * @param mags value of mag for target. This will be used for each filter in your telescope set.
 */
void Frame::addSource(double cx, double cy, double fwhm_x, double fwhm_y, double magnitude)
{
    uint16_t nfilters = tel.FGS_filter.size();
    std::vector<double> mags;
    mags.resize(nfilters);
    std::fill(mags.begin(), mags.end(), magnitude);
    addSource(cx, cy, fwhm_x, fwhm_y, mags);
}

/**
 * Add source to a Frame object to generate image data arrays. Source can be either a Guassian or a PSF, depending on what is defined in the parameters file.
 *
 * @param cx X-coordinate for source centre
 * @param cy Y-coordinate for source centre
 * @param fwhm_x source size in x direction.
 * @param fwhm_x source size in y direction.
 * @param mags array with target magnitudes. Make sure to have matching magnitudes with your telescope filter set.
 */
void Frame::addSource(double cx, double cy, double fwhm_x, double fwhm_y, std::vector<double> mags)
{
    //add a source to the list of sources. Make a temporary prob matrix.
    sources.emplace_back();

    source *src = &sources[nsources() - 1];
    Grid<double> tempProbMatrix(wsim, hsim);
    Grid<double> *probMatrix = &tempProbMatrix;

#ifdef PRINT_SOURCE_DATA
    printf("size of source array: %u \n", nsources());
#endif

    if (tel.FGS_filter.size() != mags.size())
    {
        printf("Numbers of supplied mags doesn't match number of telescope filters!!");
        double refMag = mags.at(0);
        mags.resize(tel.FGS_filter.size());
        std::fill(mags.begin(), mags.end(), refMag);
    }

    src->expected_photons = astroUtilities::meanReceivedPhotons(mags, (tel.FGS_filter), t, tel);

    src->cx = cx;
    src->cy = cy;
    src->fwhm_x = fwhm_x;
    src->fwhm_y = fwhm_y;

#ifdef PRINT_SOURCE_DATA
    printf("Average n. of photons for source %d: %3.4f\n", nsources() - 1, src->expected_photons);
#endif

    //assign source distribution array
#if SOURCE_TYPE == GAUSSIAN
    //Normalize prob to 100
    double sigmax = (fwhm_x / 2.3585) * tel.SIMELS;
    double sigmay = (fwhm_y / 2.3585) * tel.SIMELS;

    //x.0 is center of pixel. x.5 is edge
    double simcx = cx * tel.SIMELS + (tel.SIMELS / 2.0) - 0.5;
    double simcy = cy * tel.SIMELS + (tel.SIMELS / 2.0) - 0.5;
    calculateGaussian(simcx, simcy, sigmax, sigmay, probMatrix);
#endif

#ifdef PRINT_PROB_ARRAY
    PrintProbArray(probMatrix, "source");
#endif

    //add a value outside the array to the distribution func. This takes cares of photons falling outside the array.
    double prob_photon_outside_frame = 100 - probMatrix->total();
    probMatrix->operator[](probMatrix->extraPixPos()) += prob_photon_outside_frame;

    //Now assign it the vector to a distribution, and inizialize the distribution
    std::discrete_distribution<uint32_t> distribution(probMatrix->begin(), probMatrix->end());
    src->source_distribution = distribution;

    //Seec and initialise the distributions. One for total number of photons in frame, one for distribution of photons on frame.
    src->photon_n_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    src->photons = std::poisson_distribution<ulong_t>(src->expected_photons);
    src->distribution_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void Frame::generateFrame(bool statistical)
{
    for (uint16_t isrc = 0; isrc < nsources(); isrc++)
    {
        addSourcePhotons(sources[isrc]);
    }

    //Transform the simel to the actual frame
    simelsToFrame(statistical);

#ifdef DEBUG
    uint64_t tot_photons = std::accumulate(fr.begin(), fr.end(), 0.0);
    uint64_t outside_photons = fr[fr.extraPixPos()];
    printf("Total photons in frame array: %d \n", tot_photons - outside_photons);
    printf("Total photons outside frame: %d\n", outside_photons);
    printf("Saturated: %s\n", (isSaturated() ? "Yes" : "No"));
#endif
}

void Frame::reset()
{
    sources.clear();
    fr.reset();
    simfr.reset();
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
        bmpfile_header header;
        header.bmp_offset = sizeof(bmpfile_magic) + sizeof(bmpfile_header) + sizeof(bmpfile_dib_info);
        //3 bytes per pixels, plus bytes needed for packing.
        uint32_t packingSize = ((w * bytes_per_pixels) % 4) * h;
        uint32_t dataSize = w * h * bytes_per_pixels;
        header.file_size = header.bmp_offset + dataSize + packingSize;
        file.write((char *)(&header), sizeof(header));
        bmpfile_dib_info dib_info;
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
                uchar_t pixVal;
                if (fr(x, y) >= 255)
                {
                    pixVal = 255;
                }
                else
                {
                    pixVal = (uchar_t)fr(x, y);
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

void Frame::set(uint16_t initialX, uint16_t finalX, uint16_t initialY, uint16_t finalY, uint16_t value)
{
    if (finalX >= w)
    {
        finalX = w - 1;
    }

    if (finalY >= h)
    {
        finalY = h - 1;
    }

    for (int i = initialY; i <= finalY; i++)
    {
        for (int j = initialX; j <= finalX; j++)
        {
            (*this)(j, i) = value;
        }
    }
}

void Frame::setAll(uint16_t value)
{
    set(0, w - 1, 0, h - 1, value);

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
    uint16_t w = fr.width();
    uint16_t h = fr.height();
    std::cout << "Printing frame values" << std::endl;
    std::cout << "Array Width: " << w << std::endl;
    std::cout << "Array Heigth : " << h << std::endl;

    for (uint16_t y = h - 1; y == 0; y--)
    {
        for (uint16_t x = 0; x < w; x++)
        {
            printf("%d \t", fr(x, y));
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Frame::PrintProbArray(Grid<double> *probMatrixptr, const char *message)
{
    uint16_t w = probMatrixptr->width();
    uint16_t h = probMatrixptr->height();
    printf("Printing %s probability array values\n", message);
    std::cout << "Array Width: " << w << std::endl;
    std::cout << "Array Heigth : " << h << std::endl;

    for (uint16_t y = h - 1; y == 0; y--)
    {
        for (uint16_t x = 0; x < w; x++)
        {
            printf("%4.3f \t", probMatrixptr->operator()(x, y));
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Frame::PrintSimelArray()
{
    uint16_t w = simfr.width();
    uint16_t h = simfr.height();
    std::cout << "Printing simels values" << std::endl;
    std::cout << "Array Width: " << w << std::endl;
    std::cout << "Array Heigth : " << h << std::endl;

    for (uint16_t y = h - 1; y == 0; y--)
    {
        for (uint16_t x = 0; x < w; x++)
        {
            printf("%d \t", simfr(x, y));
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Frame::calculateGaussian(double cx, double cy, double sigmax, double sigmay, Grid<double> *probMatrix)
{
    uint16_t xlim = probMatrix->width();
    uint16_t ylim = probMatrix->height();
    const double A = 100 / (2 * M_PI * sigmax * sigmay);
    for (unsigned int y = 0; y < ylim; y++)
    {
        for (unsigned int x = 0; x < xlim; x++)
        {
            double xTerm = pow((x - cx), 2) / (2 * pow(sigmax, 2));
            double yTerm = pow((y - cy), 2) / (2 * pow(sigmay, 2));
            double sample_prob = (A * exp(-(xTerm + yTerm)));
            probMatrix->operator()(x, y) = sample_prob;
        }
    }
}

void Frame::addSourcePhotons(source &src)
{
    //Calculate tot photons received from source in this frame
    ulong_t number = src.frame_photons();
#ifdef DEBUG
    printf("N. photos for source in this frame: %d \n", isrc, number);
#endif

//Distribute photons
//TODO: Speed this up; limit based on saturation; saturation flag; if many photons, consider distribution more than one at a time.
#ifdef TIMING
    auto t1 = std::chrono::high_resolution_clock::now();
#endif

    while (number > 0)
    {
        simfr[src.photon_position()]++;
        number--;
    }

#ifdef TIMING
    auto t2 = std::chrono::high_resolution_clock::now();
    printf("Assign of source %f took: %f milliseconds \n", isrc, (double)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
#endif

#ifdef DEBUG
    printf("Total photons in simel array: %f \n", std::accumulate(simfr.begin(), simfr.end(), 0.0));
    printf("Total photons outside simel array: %f \n\n", simfr[simfr.extraPixPos()]);
#endif
}

void Frame::simelsToFrame(bool statistical)
{
    if (statistical == false) //just to debug, make smooth gaussian
    {
        Grid<double> tempMatrix(w, h);
        source src = sources[0];
        Grid<double> *tempMatrixPtr = &tempMatrix;
        calculateGaussian(src.cx, src.cy, src.fwhm_x / 2.3585, src.fwhm_y / 2.3585, tempMatrixPtr);
        const double A = 100 / (2 * M_PI * src.fwhm_x * src.fwhm_y);

        for (uint16_t y = 0; y < h; y++)
        {
            for (uint16_t x = 0; x < w; x++)
            {
                fr(x, y) = (uint32_t)(tempMatrix(x, y) * (50000.0 / A));
            }
        }
    }
    else
    {
        for (uint16_t y = 0; y < h; y++)
        {
            for (uint16_t x = 0; x < w; x++)
            { //Loop through image
                uint64_t pixVal = 0;
                for (uint16_t simy = y * tel.SIMELS; simy < y * tel.SIMELS + tel.SIMELS; simy++)
                {
                    for (int simx = x * tel.SIMELS; simx < x * tel.SIMELS + tel.SIMELS; simx++)
                    { //Loop through simels for selected pixel
                        if ((pixVal + simfr(simx, simy)) <= tel.FGS_MAX_ADU)
                        {
                            pixVal += simfr(simx, simy);
                        }
                        else
                        {
                            pixVal = tel.FGS_MAX_ADU;
                            saturated = true;
                        }
                    }
                }
                fr(x, y) = pixVal;
            }
        }

        fr[fr.extraPixPos()] = simfr[simfr.extraPixPos()];
    }
}
