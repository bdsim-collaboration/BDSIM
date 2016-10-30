#ifndef BDSIQUERY_H
#define BDSIQUERY_H

#include "BDSFieldClassType.hh"
#include "BDSFieldType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

class G4Field;

namespace GMAD
{
  class Query;
}

namespace BDSI
{
  void Query(G4Field* field, const GMAD::Query& params, const BDSFieldType fieldType);
  
  void Query1D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query2D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query3D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query4D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);

  void WriteOut(std::ofstream& out,
		const G4int    nDim,
		const G4double coords[4],
		const G4double result[6],
		const BDSFieldClassType type);
}

#endif
