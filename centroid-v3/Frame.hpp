/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file Frame.hpp
 * @brief Header file for Frame class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-01-20
 */

#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

const int BMP_MAGIC_ID = 2;
typedef unsigned char uchar_t;
typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef signed short int int16_t;
typedef unsigned long ulong_t;

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

//Template used to make 2D array using 1D vector.
//This contains an extra pixel at the end, used to store any extra information needed (depending on implementation).
///We use it to store pixels falling outside the image.
template <class T>
class Grid
{
public:
  Grid(unsigned int w = 0, unsigned int h = 0) : w(w), h(h), v(w * h + 1) {}

  T &operator()(unsigned int x, unsigned int y) { return v.at((y * w) + x); }
  const T &operator()(unsigned int x, unsigned int y) const { return v.at((y * w) + x); }

  T &operator[](unsigned int pos) { return v.at(pos); }
  const T &operator[](unsigned int pos) const { return v.at(pos); }

  void reserve(unsigned int w_, unsigned int h_) { v.reserve(w_ * h_ + 1); }
  void resize(unsigned int w_, unsigned int h_)
  {
    v.resize(w_ * h_ + 1);
    w = w_;
    h = h_;
  }

  auto begin() { return v.begin(); }
  auto end() { return v.end(); }
  auto size() { return v.size(); }
  T max() { return *std::max_element(begin(), end()); }
  T min() { return *std::min_element(begin(), end()); }
  T total()
  {
    return std::accumulate(v.begin(), v.end(), 0.0);
  }

  //might need to delete following line.
  std::vector<T> &vector()
  {
    return v;
  }

  unsigned int width() const { return w; }
  unsigned int height() const { return h; }
  unsigned int extraPixPosition() const { return w * h; }

private:
  unsigned int w, h;
  std::vector<T> v;
};

struct source
{
  std::default_random_engine photon_n_generator;
  //std::default_random_engine distribution_generator;
  std::mt19937 distribution_generator;

  std::poisson_distribution<ulong_t> photons;
  std::discrete_distribution<uint32_t> source_distribution;
  Grid<double> source_prob_matrix;

  ulong_t frame_photons()
  {
    return photons(photon_n_generator);
  }

  uint32_t photon_position()
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
  //Redirect to main Constructor
  Frame(double expTime, int width, int height) : Frame(expTime, (unsigned int)width, (unsigned int)height){};

  //Main Constructor
  Frame(double expTime = 1.0, unsigned int width = 1024, unsigned int height = 1024);
  ~Frame();

  unsigned int &operator()(unsigned int x, unsigned int y);
  const unsigned int &operator()(unsigned int x, unsigned int y) const;

  //Redirect first method to second, generic one
  void addSource(double cx, double cy, double wx, double wy, double magnitude)
  {
    addSource(cx, cy, wx, wy, magnitude, magnitude, magnitude);
  }
  void addSource(double cx, double cy, double wx, double wy, double magB, double magV, double magR);

  void generateFrame();

  void saveToBitmap(std::string filename);
  void saveToFile(std::string filename);
  void setAll(unsigned int value);
  void Print();

private:
  double mag, t;
  source *src = new source;
  double expected_photons;
  int h, w;
  Grid<uint32_t> fr;
};