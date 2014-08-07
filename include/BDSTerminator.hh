/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#ifndef BDSTerminator_h
#define BDSTerminator_h

#include "BDSAcceleratorComponent.hh"
#include "parser/elementlist.h"

class BDSTerminator : public BDSAcceleratorComponent
{
public:
  BDSTerminator(G4String aName, G4double aLength);
  ~BDSTerminator();
private:
  void TerminatorLogicalVolume();

  //id of this sampler / terminator
  G4int nThisSampler;
  /// total number of terminators
  static G4int nSamplers;

  G4VisAttributes* SetVisAttributes();
};

void AddTerminatorToEndOfBeamline(ElementList* beamline_list);

#endif
