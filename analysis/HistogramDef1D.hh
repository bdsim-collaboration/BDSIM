#ifndef HISTOGRAMDEF_H
#define HISTOGRAMDEF_H

#include <string>

class HistogramDef1D
{
public:
  HistogramDef1D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 std::string plotIn,
		 std::string selectionIn = "1");
  virtual ~HistogramDef1D(){;}
  
  std::string treeName;
  std::string histName;
  int         xNBins;
  double      xLow;
  double      xHigh;
  std::string plot;
  std::string selection;

private:
  /// No default constructor
  HistogramDef1D() = delete;
};

#endif
