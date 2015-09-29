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
  G4String fileName;

  BDSGeometry* BuildGmad();
  
#ifdef USE_LCDD
  BDSGeometry* BuildLCDD();
#endif
  
  BDSGeometry* BuildMokka();
  
#ifdef USE_GDML
  BDSGeometry* BuildGDML();
#endif
};

#endif
