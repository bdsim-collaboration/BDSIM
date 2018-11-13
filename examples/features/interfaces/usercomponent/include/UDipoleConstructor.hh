#ifndef UDIPOLECONSTRUCTOR_H
#define UDIPOLECONSTRUCTOR_H

#include "BDSComponentConstructor.hh"

class UDipoleConstructor: public BDSComponentConstructor
{
public:
  UDipoleConstructor(){;}
  virtual ~UDipoleConstructor(){;}

  virtual BDSAcceleratorComponent* Construct(GMAD::Element const* elementIn,
					     GMAD::Element const* prevElementIn,
					     GMAD::Element const* nextElementIn,
					     G4double currentArcLengthIn,
					     G4double brhoIn,
					     G4double beta0In);
};

#endif
