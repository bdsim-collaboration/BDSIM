#ifndef BDSGeometryInterface_h
#define BDSGeometryInterface_h 1

#include "globals.hh"

/** Geometry Interface Class
//========================
//
// A class of functions to output Geant4/Mokka/BDSIM parameters for elements
// - to include geometry, optics, fields, etc.
*/

class BDSGeometryInterface
{
public:
  BDSGeometryInterface(G4String filename);  
  ~BDSGeometryInterface();

  void Optics();
  void Survey();
  
protected:

  G4String itsFileName;

private:

};

#endif
