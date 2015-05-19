#ifndef __BDSMAGFIELDFACTORY_
#define __BDSMAGFIELDFACTORY_

#include "globals.hh"
#include "BDSMagField.hh"

class BDSMagFieldFactory{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  BDSMagField* buildMagField(G4String bmap, G4double bmapZOffset);
 
private:
  G4String _bmap;
  G4String _bFile;
  G4String _bFormat;
  G4double _bmapZOffset;

  void parseFormatAndFilename();

  BDSMagField* build3DMagField();
  BDSMagField* buildXYMagField();
};
#endif
