#include "HistogramDef1D.hh"

HistogramDef1D::HistogramDef1D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       std::string variableIn,
			       std::string selectionIn):
  HistogramDef(treeNameIn, histNameIn, 1, variableIn, selectionIn),
  xNBins(xNBinsIn),
  xLow(xLowIn),
  xHigh(xHighIn)
{;}
