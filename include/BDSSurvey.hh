#ifndef BDSSurvey_h
#define BDSSurvey_h 1

#include "globals.hh"

/** Geometry Interface Class
 * @brief A class of functions to output Geant4/Mokka/BDSIM parameters for elements
 * to include geometry, optics, fields, etc.
*/

class BDSSurvey
{
public:
  BDSSurvey(G4String filename);  
  ~BDSSurvey();

  void Optics();
  void Survey();
  
protected:

  G4String itsFileName;

private:

};

#endif
