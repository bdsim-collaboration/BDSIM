#ifndef BDSIQUERY_H
#define BDSIQUERY_H

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

class G4Field;

namespace GMAD
{
  class Query;
}

namespace BDSI
{
  void Query(G4Field* field, const GMAD::Query& params);
  
  void Query1D(G4Field* field, const GMAD::Query& params);
  void Query2D(G4Field* field, const GMAD::Query& params);
  void Query3D(G4Field* field, const GMAD::Query& params);
  void Query4D(G4Field* field, const GMAD::Query& params);
}

#endif
