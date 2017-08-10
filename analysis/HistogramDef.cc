#include "HistogramDef.hh"

HistogramDef::HistogramDef(std::string treeNameIn,
			   std::string histNameIn,
			   int         nDimensionsIn,
			   std::string variableIn,
			   std::string selectionIn):
  treeName(treeNameIn),
  histName(histNameIn),
  nDimensions(nDimensionsIn),
  variable(variableIn),
  selection(selectionIn)
{;}
