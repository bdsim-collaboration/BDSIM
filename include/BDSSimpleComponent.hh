#ifndef BDSSIMPLECOMPONENT_H
#define BDSSIMPLECOMPONENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSGeometryComponent;

class BDSSimpleComponent: public BDSAcceleratorComponent
{
public:
  BDSSimpleComponent(G4String              name,
		     BDSGeometryComponent* componentIn,
		     G4double              angle = 0);

  virtual ~BDSSimpleComponent(){;}

private:
  BDSSimpleComponent();

  /// Required implementation from base class.
  virtual void BuildContainerLogicalVolume(){;}
};

#endif
