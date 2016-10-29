#ifndef BDSIQUERY_H
#define BDSIQUERY_H

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

class G4Field;

namespace BDSI
{
  void Query2D(G4Field* field,
	       G4String outputName,
	       G4int    nX,   G4int    nY,
	       G4double xmin, G4double xmax,
	       G4double ymin, G4double ymax);

}

#endif
