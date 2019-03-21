
//Template used to make 2D array using 1D vector.
//This contains an extra pixel at the end, used to store any extra information needed (depending on implementation).
///We use it to store pixels falling outside the image.
#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
//#include "Config/parameters.h"

template <class T>
class Grid
{
public:
  Grid(unsigned int w = 0, unsigned int h = 0) : w(w), h(h), v(w * h + 1), extraPixPosition(w * h)
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

  //might need to delete following line.
  std::vector<T> &vector()
  {
    return v;
  }

  unsigned int width() const { return w; }
  unsigned int height() const { return h; }
  unsigned int extraPixPos() const { return extraPixPosition; };

private:
  unsigned int w, h, extraPixPosition;
  std::vector<T> v;
};