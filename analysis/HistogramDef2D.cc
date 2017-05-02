#include "HistogramDef2D.hh"


HistogramDef2D::HistogramDef2D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       int         yNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       double      yLowIn,
			       double      yHighIn,
			       std::string plotIn,
			       std::string selectionIn):
  HistogramDef1D(treeNameIn, histNameIn, xNBinsIn, xLowIn,
		 xHighIn, plotIn, selectionIn),
  yNBins(yNBinsIn),
  yLow(yLowIn),
  yHigh(yHighIn)
{;}
