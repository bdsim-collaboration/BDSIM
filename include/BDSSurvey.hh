#ifndef BDSSurvey_h
#define BDSSurvey_h 1

#include "globals.hh"

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

  
private:

  /// write survey to file
  void Survey();
  /// filename
  G4String itsFileName;
};

#endif
