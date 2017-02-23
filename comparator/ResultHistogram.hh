#ifndef COMPRESULTHISTOGRAM_H
#define COMPRESULTHISTOGRAM_H

#include "Result.hh"

#include <iomanip>
#include <sstream>
#include <string>

/**
 * @brief Result of comparing 2 histograms.
 *
 * @author Stewart Boogert.
 */

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
  double tolerance;

  virtual std::string print() const
  {
    const int nw = 15;
    std::stringstream ss;
    ss << Result::print();
    ss << "Parameter" << std::setw(nw) << std::right << "file1"    << " / "
       << std::left   << std::setw(nw) << "file2"    << "\n";
    ss << "Entries  " << std::setw(nw) << std::right << h1Entries  << " / "
       << std::left   << std::setw(nw) << h2Entries  << "\n";
    ss << "Bins     " << std::setw(nw) << std::right << h1NXBins   << " / "
       << std::left   << std::setw(nw) << h2NXBins   << "\n";
    ss << "X Mean   " << std::setw(nw) << std::right << h1XMean    << " / "
       << std::left   << std::setw(nw) << h2XMean    << "\n";
    ss << "X RMS    " << std::setw(nw) << std::right << h1XRms     << " / "
       << std::left   << std::setw(nw) << h2XRms     << "\n";
    ss << "Integral " << std::setw(nw) << std::right << h1Integral << " / "
       << std::left   << std::setw(nw) << h2Integral << "\n";
    ss << "Chi^2    " << chi2 << " vs tolerance of " << tolerance;
    return ss.str();
  }

  virtual ~ResultHistogram() {}
};

#endif
