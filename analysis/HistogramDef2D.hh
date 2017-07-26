#ifndef HISTOGRAMDEF2D_H
#define HISTOGRAMDEF2D_H

#include "HistogramDef1D.hh"

#include <string>

/**
 * @brief Specification for 2D histogram.
 *
 * @author L. Nevay
 */

class HistogramDef2D: public HistogramDef1D
{
public:
  HistogramDef2D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 int         yNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 double      yLowIn,
		 double      yHighIn,
		 std::string plotIn,
		 std::string selectionIn = "1");
  virtual ~HistogramDef2D(){;}

  int         yNBins;
  double      yLow;
  double      yHigh;

private:
  /// No default constructor
  HistogramDef2D() = delete;
};

#endif
