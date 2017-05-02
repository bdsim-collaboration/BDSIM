#include "HistogramDef1D.hh"

HistogramDef1D::HistogramDef1D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       std::string plotIn,
			       std::string selectionIn):
  treeName(treeNameIn),
  histName(histNameIn),
  xNBins(xNBinsIn),
  xLow(xLowIn),
  xHigh(xHighIn),
  plot(plotIn),
  selection(selectionIn)
{;}
