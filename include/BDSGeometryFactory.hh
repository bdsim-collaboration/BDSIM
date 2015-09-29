#ifndef BDSGEOMETRYFACTORY_H
#define BDSGEOMETRYFACTORY_H

#include "globals.hh"

class BDSGeometry;

class BDSGeometryFactory
{
public:
  BDSGeometryFactory();
  ~BDSGeometryFactory();
  
  BDSGeometry* BuildGeometry(G4String formatAndFilePath);
 
private:
  BDSGeometry* BuildGmad(G4String fileName);
  
#ifdef USE_LCDD
  BDSGeometry* BuildLCDD(G4String fileName);
#endif
  
  BDSGeometry* BuildMokka(G4String fileName);
  
#ifdef USE_GDML
  BDSGeometry* BuildGDML(G4String fileName);
#endif
};

#endif
