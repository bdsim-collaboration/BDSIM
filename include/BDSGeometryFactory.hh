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
  BDSGeometry* buildLCDD();
  BDSGeometry* buildMokka();
  BDSGeometry* buildGDML();
  BDSGeometry* buildNone();
};
#endif
