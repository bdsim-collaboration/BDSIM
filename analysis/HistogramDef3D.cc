#include "HistogramDef3D.hh"


HistogramDef3D::HistogramDef3D(std::string treeNameIn,
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
			       std::string selectionIn):
  HistogramDef2D(treeNameIn, histNameIn, xNBinsIn, yNBinsIn,
		 xLowIn, xHighIn, yLowIn, yHighIn,
		 plotIn, selectionIn),
  zNBins(zNBinsIn),
  zLow(zLowIn),
  zHigh(zHighIn)
{
  nDimensions = 3;
}
