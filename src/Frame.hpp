/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 *
 * @file Frame.hpp
 * @brief Header file for Frame class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-01-20
 */
#pragma once

#include <random>
#include <chrono>
#include <memory>

#include "typedefs.h"
#include "Grid.hpp"
#include "telescopes.h"

const int BMP_MAGIC_ID = 2;

// --------------------------------------------------------------
// Windows BMP-specific format data
struct bmpfile_magic
{
  uchar_t magic[BMP_MAGIC_ID];
};

struct bmpfile_header
{
  uint32_t file_size;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
};

struct bmpfile_dib_info
{
  uint32_t header_size;
  int32_t width;
  int32_t height;
  uint16_t num_planes;
  uint16_t bits_per_pixel;
  uint32_t compression;
  uint32_t bmp_byte_size;
  int32_t hres;
  int32_t vres;
  uint32_t num_colors;
  uint32_t num_important_colors;
};

struct source
{
  // std::default_random_engine photon_n_generator;
  std::default_random_engine ADUs_n_generator, bias_generator;
  // std::default_random_engine distribution_generator;
  std::mt19937 distribution_generator;

  // std::poisson_distribution<ulong_t> photons;
  std::poisson_distribution<ulong_t> ADUs;
  std::discrete_distribution<uint32_t> source_distribution;
  // double expected_photons;
  double expected_ADUs;
  double cx, cy, fwhm_x, fwhm_y;

  /*ulong_t frame_photons()
  {
    return photons(photon_n_generator);
  }*/

  ulong_t frame_ADUs()
  {
    return ADUs(ADUs_n_generator);
  }

  uint32_t detection_position()
  {
    return source_distribution(distribution_generator);
  }
};

/**
 * Header file for Frame class. Takes a star magnitude and exposure time, and simulates picture frames. This includes subpixel arrays and sub exposures arrays.
 * Gaussian or Zemax PSF are controlled from the parameters file.
 *
 * @brief Class to generated image frames
 */
class Frame
{
public:
  // Main Constructor
  Frame(Telescope _tel, double _expTime = 0.0, Grid<uint32_t> _grid = Grid<uint32_t>(0, 0));

  ~Frame();

  uint32_t &operator()(uint16_t x, uint16_t y);
  const uint32_t &operator()(uint16_t x, uint16_t y) const;

  // Redirect first method to second, generic one
  void addSource(double cx, double cy, double fwhm_x, double fwhm_y, double magnitude);
  void addSource(double cx, double cy, double fwhm_x, double fwhm_y, std::vector<double> mags);

  void generateFrame(bool statistical = true);
  void reset();

  void saveToBitmap(std::string filename);
  void saveToFile(std::string filename);
  void Print();
  void PrintSimelArray();

  void set(uint16_t initialX, uint16_t finalX, uint16_t initialY, uint16_t finalY, uint16_t value);
  void setAll(uint16_t value);

  // TODO: return smart pointer
  void subFrame(uint16_t centerX, uint16_t centerY, uint16_t width, uint16_t height);

  bool isSaturated()
  {
    return saturated;
  };
  // std::shared_ptr<int> const get() const { return std::shared_ptr<}
  Grid<uint32_t> const *get() const
  {
    return &fr;
  }
  // const Grid<uint32_t> getGrid() { return fr; }

  inline std::shared_ptr<Grid<uint32_t>> get_smartPtr()
  {
    std::shared_ptr<Grid<uint32_t>> m_fr = std::make_shared<Grid<uint32_t>>(fr);
    return m_fr;
  }

private:
  Telescope tel;
  double mag, t;
  bool saturated = false;

  std::default_random_engine readnoise_generator, dark_generator;
  std::poisson_distribution<ulong_t> darkCounts;
  std::normal_distribution<double> readnoiseCounts;

  ulong_t pixel_darkCounts()
  {
    return darkCounts(dark_generator);
  }
  ulong_t pixel_readnoiseCounts()
  {
    double readnoise = readnoiseCounts(readnoise_generator);
    if (readnoise < 0) readnoise = 0;
    return (ulong_t)std::round(readnoise);
  }

  std::vector<source> sources;
  uint32_t h, w, hsim, wsim;
  Grid<uint32_t> simfr, fr;

  uint16_t nsources()
  {
    return sources.size();
  }
  void calculateGaussian(double cx, double cy, double sigmax, double sigmay,
                         Grid<double> *probMatrix);
  void addDarkNoise();
  void addBiasNoise();
  void addPedestal(uint16_t value);

  void PrintProbArray(Grid<double> *probMatrixptr, const char *message);
  void simelsToFrame(bool statistical = true);
  void addSourceDetections(source &src, ulong_t totDetections);
};