#ifndef __BDSMAGFIELDFACTORY_
#define __BDSMAGFIELDFACTORY_

#include "globals.hh"
#include "BDSMagField.hh"
#include "BDSBmapFormat.hh"

class BDSMagFieldFactory{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  BDSMagField* buildMagField(G4String bmap, G4double bmapZOffset);
 
private:
  G4String _bmap;
  G4String _bFile;
  BDSBmapFormat* _bFormat;
  G4double _bmapZOffset;

  void parseFormatAndFilename();

  BDSMagField* build3DMagField();
  BDSMagField* buildXYMagField();
  BDSMagField* buildZeroMagField();
};
#endif
