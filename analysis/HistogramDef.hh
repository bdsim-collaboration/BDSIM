#ifndef HISTOGRAMDEF_H
#define HISTOGRAMDEF_H

#include <string>

/**
 * @brief Common specification for a histogram.
 *
 * @author L. Nevay
 */

class HistogramDef
{
public:
  HistogramDef(std::string treeNameIn,
	       std::string histNameIn,
	       int         nDimensionsIn,
               std::string variable,
	       std::string selectionIn = "1");
  virtual ~HistogramDef(){;}
  
  std::string treeName;
  std::string histName;
  int         nDimensions;
  std::string variable;
  std::string selection;

private:
  /// No default constructor
  HistogramDef() = delete;
};

#endif
