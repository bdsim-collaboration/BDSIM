#ifndef COMPRESULTHISTOGRAM2D_H
#define COMPRESULTHISTOGRAM2D_H

#include "ResultHistogram.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparing 2 2D histograms.
 *
 * @author Stewart Boogert.
 */

class ResultHistogram2D: public ResultHistogram
{
public:
  int    h1NYBins;
  int    h2NYBins;
  double h1YMean;
  double h2YMean;
  double h1YRms;
  double h2YRms;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << ResultHistogram::print();
    ss << h1NYBins << " " << h2NYBins << " " << h1YMean << " " << h2YMean << " ";
    return ss.str();
  }

  virtual ~ResultHistogram2D() {}
};

#endif
