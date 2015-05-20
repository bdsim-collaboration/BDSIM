#ifndef __BDSMAGFIELDFACTORY_
#define __BDSMAGFIELDFACTORY_

#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4CachedMagneticField.hh"
#include "BDSBmapFormat.hh"
#include "BDSGeometry.hh"

class BDSMagFieldFactory{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  G4MagneticField* buildMagField(G4String bmap, G4double bmapZOffset, BDSGeometry* geom);
 
private:
  G4String _bmap;
  G4String _bFile;
  BDSBmapFormat* _bFormat;
  BDSGeometry* _geom;
  G4double _bmapZOffset;
  G4double _cacheLength;

  void parseFormatAndFilename();

  G4MagneticField* build3DMagField();
  G4MagneticField* buildXYMagField();
  G4MagneticField* buildLCDDMagField();
  G4MagneticField* buildSQLMagField();
  G4MagneticField* buildZeroMagField();
};
#endif
