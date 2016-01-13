#ifndef BDSSURVEY_H
#define BDSSURVEY_H

#include <fstream>
#include <vector>
#include "globals.hh"

namespace GMAD {
  struct Element;
}

class BDSBeamline;
class BDSBeamlineElement;

/** Survey Class
 * @brief A class of functions to output Geant4/Mokka/BDSIM parameters for elements
 * to include geometry, optics, fields, etc.
 * much like MadX Tfs file
 */

class BDSSurvey
{
public:
  BDSSurvey(G4String filename);  
  ~BDSSurvey();

  /// write header
  void WriteHeader();
  /// write line
  void Write(BDSBeamlineElement* component, GMAD::Element & element);
  /// write multiple lines
  void Write(std::vector<BDSBeamlineElement*> components, GMAD::Element & element);
  /// write summary
  void WriteSummary(BDSBeamline* beamline);
  
private:

  /// output file stream
  std::ofstream survey;

};

#endif
