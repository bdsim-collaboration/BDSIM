#ifndef __BDSGEOMETRYFACTORY_
#define __BDSGEOMETRYFACTORY_

#include "globals.hh"
#include "BDSGeometry.hh"
#include "BDSGeometryFormat.hh"

class BDSGeometryFactory{
public:
  BDSGeometryFactory();
  ~BDSGeometryFactory();
  
  BDSGeometry* buildGeometry(G4String geometry);
 
private:
  G4String _geometry;
  G4String _gFile;
  BDSGeometryFormat* _gFormat;

  void parseFormatAndFilename();

  BDSGeometry* buildGmad();
#ifdef USE_LCDD
  BDSGeometry* buildLCDD();
#endif
  BDSGeometry* buildMokka();
#ifdef USE_GDML
  BDSGeometry* buildGDML();
#endif
  BDSGeometry* buildNone();
};
#endif
