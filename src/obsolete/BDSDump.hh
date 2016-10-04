#ifndef BDSDUMP_H
#define BDSDUMP_H 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSDump: public BDSAcceleratorComponent
{
public:
  BDSDump(G4String aName,
	  G4double aLength);
  ~BDSDump();

private:
  virtual void BuildContainerLogicalVolume();

  /// number of total Dumps
  static int nDumps;
};

#endif
