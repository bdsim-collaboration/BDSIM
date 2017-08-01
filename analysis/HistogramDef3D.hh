#ifndef HISTOGRAMDEF3D_H
#define HISTOGRAMDEF3D_H

#include "HistogramDef2D.hh"

#include <string>

/**
 * @brief Specification for 3D Histogram.
 *
 * @author L. Nevay
 */

class HistogramDef3D: public HistogramDef2D
{
public:
  HistogramDef3D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 int         yNBinsIn,
		 int         zNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 double      yLowIn,
		 double      yHighIn,
		 double      zLowIn,
		 double      zHighIn,
		 std::string plotIn,
		 std::string selectionIn = "1");
  virtual ~HistogramDef3D(){;}

  int         zNBins;
  double      zLow;
  double      zHigh;

private:
  /// No default constructor
  HistogramDef3D() = delete;
};

#endif
