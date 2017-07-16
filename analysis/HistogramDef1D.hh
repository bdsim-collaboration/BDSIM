#ifndef HISTOGRAMDEF1D_H
#define HISTOGRAMDEF1D_H

#include "HistogramDef.hh"

#include <string>

/**
 * @brief Specification for 1D histogram.
 *
 * @author L. Nevay
 */

class HistogramDef1D: public HistogramDef
{
public:
  HistogramDef1D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 std::string variableIn,
		 std::string selectionIn = "1");
  virtual ~HistogramDef1D(){;}
  
  int         xNBins;
  double      xLow;
  double      xHigh;

private:
  /// No default constructor
  HistogramDef1D() = delete;
};

#endif
