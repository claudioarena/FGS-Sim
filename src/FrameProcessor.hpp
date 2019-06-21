/**
 * Twinkle FGS-Sim: Centroid recovery simulation
 * 
 * @file FrameProcessor.hpp
 * @brief Header file for FrameProcessor class
 * @author Claudio Arena, Feiyu Fang
 * @version 1.0.0 2019-03-15
 */
#pragma once

#include "Grid.hpp"
#include "typedefs.h"

class FrameProcessor
{
public:
  enum backgroundMethod : uint8_t
  {
    Random_Global,
    Border
  };

  //Main constructor
  FrameProcessor(const Grid<uint32_t> *const _frame) : frame(_frame)
  {
#ifdef DEBUG_MEMORY
    std::cout << "Make Frame Processor " << std::endl;
#endif
  };

  ~FrameProcessor();
  const static pixel_coordinates momentum(const Grid<uint32_t> *fr, uint16_t threshold = 0);
  const pixel_coordinates momentum(uint16_t threshold = 0) const;

  const static pixel_coordinates initial_guess_momentum(const Grid<uint32_t> *fr, uint16_t sigma_threshold = 4, uint8_t background_method = Random_Global);
  const pixel_coordinates initial_guess_momentum(uint16_t sigma_threshold = 4, uint8_t background_method = Random_Global) const;

  const pixel_coordinates multiple_guess_momentum(uint16_t minWindowSize, uint16_t sigma_threshold, uint16_t sigma_threshold_final) const;
  const static pixel_coordinates multiple_guess_momentum(const Grid<uint32_t> *fr, uint16_t minWindowSize, uint16_t sigma_threshold, uint16_t sigma_threshold_final);

  const pixel_coordinates fine_momentum(double guessX, double guessY, uint16_t windowSize, uint16_t sigma_threshold) const;
  const static pixel_coordinates fine_momentum(const Grid<uint32_t> *fr, double guessX, double guessY, uint16_t windowSize, uint16_t sigma_threshold);

  uint64_t static total(const Grid<uint32_t> *fr, uint16_t threshold = 0);
  uint64_t total(uint16_t threshold = 0) const;

  const std::vector<uint64_t> sumVertical(uint16_t threshold = 0) const;
  const std::vector<uint64_t> sumVertical(uint16_t initialPos, uint16_t finalPos, uint16_t threshold = 0) const;
  const static std::vector<uint64_t> sumVertical(const Grid<uint32_t> *fr, uint16_t threshold = 0);
  const static std::vector<uint64_t> sumVertical(const Grid<uint32_t> *fr, uint16_t initialPos, uint16_t finalPos, uint16_t threshold = 0);

  const std::vector<uint64_t> sumHorizontal(uint16_t threshold = 0) const;
  const std::vector<uint64_t> sumHorizontal(uint16_t initialPos, uint16_t finalPos, uint16_t threshold = 0) const;
  const static std::vector<uint64_t> sumHorizontal(const Grid<uint32_t> *fr, uint16_t threshold = 0);
  const static std::vector<uint64_t> sumHorizontal(const Grid<uint32_t> *fr, uint16_t initialPos, uint16_t finalPos, uint16_t threshold = 0);

  const uint32_t &operator()(unsigned int x, unsigned int y) const;
  const double backgroundLevel(uint8_t method = Random_Global) const;
  const static double backgroundLevel(const Grid<uint32_t> *fr, uint8_t method = Random_Global);

private:
  const Grid<uint32_t> *frame;
};