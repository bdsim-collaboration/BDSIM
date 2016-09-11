#ifndef COMPRESULTHISTOGRAM_H
#define COMPRESULTHISTOGRAM_H

#include "Result.hh"

#include <sstream>
#include <string>

class ResultHistogram: public Result
{ 
public:
  double h1Entries;
  double h2Entries; 
  int    h1NXBins;
  int    h2NXBins;
  double h1XMean;
  double h2XMean;
  double h1XRms;
  double h2XRms;
  double h1Integral;
  double h2Integral;
  double chi2;

  virtual std::string print() const
  {
    std::stringstream ss;
    ss << Result::print();
    ss << h1Entries  << "(" << h2Entries  << ") " << h1NXBins << "(" << h2NXBins << ") "
       << h1XMean    << "(" << h2XMean    << ") " << h1XRms   << "(" << h2XRms   << ") " 
       << h1Integral << "(" << h2Integral << ") " << chi2;
    return ss.str();
  }
};

#endif
