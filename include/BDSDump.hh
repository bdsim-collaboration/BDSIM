//  
//   BDSIM, (C) 2001-2006 
//    
//   version 0.3 
//   last modified : 08 May 2007 by agapov@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange with external codes
//


#ifndef BDSDump_h
#define BDSDump_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSDump : public BDSAcceleratorComponent
{
public:
  BDSDump(G4String aName,G4double aLength, G4String aTunnelMaterial="");
  ~BDSDump();

private:

  virtual void BuildMarkerLogicalVolume();
  virtual void SetVisAttributes();

  /// number of total Dumps
  static int nDumps;
};

#endif
