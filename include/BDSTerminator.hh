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
  virtual void BuildContainerLogicalVolume();
};

void AddTerminatorToEndOfBeamline(ElementList* beamline_list);

#endif
