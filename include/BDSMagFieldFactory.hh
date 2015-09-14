#ifndef BDSMAGFIELDFACTORY_H
#define BDSMAGFIELDFACTORY_H

#include "globals.hh"
#include "BDSBmapFormat.hh"
#include "BDSGeometry.hh"
#include "BDSMagField.hh"

class BDSMagFieldFactory
{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  BDSMagField* BuildMagneticField(G4String bmap         = "",
				  G4double bmapZOffset  = 0,
				  BDSGeometry* geometry = nullptr,
				  G4double bmapXOffset  = 0);
 
private:
  G4String       bmap;
  G4String       bFile;
  BDSBmapFormat* bFormat;
  BDSGeometry*   geom;
  G4double       bmapZOffset;
  G4double       bmapXOffset;
  G4double       cacheLength;

  void parseFormatAndFilename();

  BDSMagField* Build3DMagField();
  BDSMagField* BuildXYMagField();
  BDSMagField* BuildLCDDMagField();
  BDSMagField* BuildSQLMagField();
  BDSMagField* BuildZeroMagField();
};
#endif
