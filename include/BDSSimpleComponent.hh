#ifndef BDSSIMPLECOMPONENT_H
#define BDSSIMPLECOMPONENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSGeometryComponent;

/**
 * @brief A BDSAcceleratorComponent wrapper for BDSGeometryComponent.
 * 
 * This allows a BDSGeometryComponent to be placed in a beam line by
 * being a necessary BDSAcceleratorComponent, which gives the minimal 
 * information to BDSBeamline - name, length & angle.
 * 
 * This inherits the objects from the BDSGeometryComponent and therefore
 * effectively owns it without deleting it explicitly as a whole object.
 *
 * @author Laurie Nevay
 */

class BDSSimpleComponent: public BDSAcceleratorComponent
{
public:
  BDSSimpleComponent(G4String              name,
		     BDSGeometryComponent* componentIn,
		     G4double              length,
		     G4double              angle = 0);

  /// Default destructor suffices as this calls base class which
  /// clears everything up.
  virtual ~BDSSimpleComponent(){;}

private:
  /// Private default constructor to force use of given one.
  BDSSimpleComponent();

  /// Required implementation from base class.
  virtual void BuildContainerLogicalVolume(){;}
};

#endif
