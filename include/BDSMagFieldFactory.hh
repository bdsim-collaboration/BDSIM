#ifndef __BDSMAGFIELDFACTORY_
#define __BDSMAGFIELDFACTORY_

#include "globals.hh"
#include "BDSBmapFormat.hh"
#include "BDSGeometry.hh"
#include "BDSMagField.hh"

class BDSMagFieldFactory{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  BDSMagField* buildMagField(G4String bmap, G4double bmapZOffset, BDSGeometry* geom);
 
private:
  G4String _bmap;
  G4String _bFile;
  BDSBmapFormat* _bFormat;
  BDSGeometry* _geom;
  G4double _bmapZOffset;
  G4double _cacheLength;

  void parseFormatAndFilename();

  BDSMagField* build3DMagField();
  BDSMagField* buildXYMagField();
  BDSMagField* buildLCDDMagField();
  BDSMagField* buildSQLMagField();
  BDSMagField* buildZeroMagField();
};
#endif
