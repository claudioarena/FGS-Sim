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
  //Main constructor
  FrameProcessor(const Grid<uint32_t> *const _frame) : frame(_frame)
  {
#ifdef DEBUG_MEMORY
    std::cout << "Make Frame Processor " << std::endl;
#endif
  };

  ~FrameProcessor();
  const centroid momentum() const;

  uint64_t total() const;
  const std::vector<uint64_t> sumVertical() const;
  const std::vector<uint64_t> sumVertical(uint16_t initialPos, uint16_t finalPos) const;
  const std::vector<uint64_t> sumHorizontal() const;
  const std::vector<uint64_t> sumHorizontal(uint16_t initialPos, uint16_t finalPos) const;

  const uint32_t &operator()(unsigned int x, unsigned int y) const;

private:
  const Grid<uint32_t> *frame;
};