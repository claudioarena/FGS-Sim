
//Template used to make 2D array using 1D vector.
//This contains an extra pixel at the end, used to store any extra information needed (depending on implementation).
///We use it to store pixels falling outside the image.
#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
//#include "Config/parameters.h"
#include "typedefs.h"

template <class T>
class Grid
{
public:
  Grid(uint16_t w = 0, uint16_t h = 0) : w(w), h(h), extraPixPosition(w * h), v(w * h + 1)
  {
#ifdef DEBUG_MEMORY
    std::cout << "Created Grid " << std::endl;
#endif
  }

  ~Grid()
  {
#ifdef DEBUG_MEMORY
    std::cout << "Destroyed Grid" << std::endl;
#endif
  }

  T &operator()(uint16_t x, uint16_t y) { return v.at((y * w) + x); }
  const T &operator()(uint16_t x, uint16_t y) const { return v.at((y * w) + x); }

  T &operator[](uint32_t pos) { return v.at(pos); }
  const T &operator[](uint32_t pos) const { return v.at(pos); }

  void reserve(uint16_t w_, uint16_t h_) { v.reserve(w_ * h_ + 1); }
  void resize(uint16_t w_, uint16_t h_)
  {
    v.resize(w_ * h_ + 1);
    w = w_;
    h = h_;
    extraPixPosition = w * h;
  }

  auto begin() const { return v.begin(); }
  auto end() const { return v.end(); }
  auto size() { return v.size(); }
  T max() { return *std::max_element(begin(), end()); }
  T min() { return *std::min_element(begin(), end()); }
  T total()
  {
    return std::accumulate(v.begin(), v.end(), 0.0);
  }
  void reset() { std::fill(v.begin(), v.end(), 0); }
  void clear() { v.clear(); }

  //might need to delete following line.
  std::vector<T> &vector()
  {
    return v;
  }

  //TODO: might be better to return a smart pointer?

  //This returns a subgrid. Also, increments the offset values passed, to keep track of original coordinates
  Grid<T> subGrid(uint16_t centerX, uint16_t centerY, uint16_t width, uint16_t height, uint16_t *resultOffsetX = NULL, uint16_t *resultOffsetY = NULL)
  {
    if (width == 0 || height == 0)
    {
      (*resultOffsetX) = 0;
      (*resultOffsetY) = 0;
      return Grid<T>(1, 1);
    }

    //if even size, we can't have a center pixel. Make it odd
    if ((width % 2) == 0)
      width++;
    if ((height % 2) == 0)
      height++;

    int32_t minX = (int32_t)centerX - ((width - 1) / 2);
    int32_t minY = (int32_t)centerY - ((height - 1) / 2);
    int32_t maxX = minX + width - 1;
    int32_t maxY = minY + height - 1;

    if (minX < 0)
      minX = 0;
    if (minY < 0)
      minY = 0;
    if (maxX >= w)
      maxX = w - 1;
    if (maxY >= h)
      maxY = h - 1;

    width = (maxX - minX) + 1;
    height = (maxY - minY) + 1;
    Grid<T> subGrid = Grid<T>(width, height);

    for (uint16_t x = 0; x < width; x++)
    {
      for (uint16_t y = 0; y < height; y++)
      {
        subGrid(x, y) = operator()(x + minX, y + minY);
      }
    }

    *resultOffsetX += (uint16_t)minX;
    *resultOffsetY += (uint16_t)minY;
    return subGrid;
  }

  unsigned int width() const { return w; }
  unsigned int height() const { return h; }
  unsigned int extraPixPos() const { return extraPixPosition; };

private:
  uint32_t w, h, extraPixPosition;
  std::vector<T> v;
};