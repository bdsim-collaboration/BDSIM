#ifndef BDSELEMENT_H
#define BDSELEMENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

/**
 * @brief A class for a generic piece of external geometry.
 * 
 * Allows any arbritary geometry and magnetic field map to be used
 * as an accelerator component in the beamline. Geometry and magnetic fields are imported
 * from an external file (each) and can be specified in various formats.
 */

class BDSElement: public BDSAcceleratorComponent
{
public:
  BDSElement(G4String name,
	     G4double length,
	     G4double outerDiameterIn,
	     G4String geometry,
	     G4String bmap);
  virtual ~BDSElement();
   
private:
  /// Required implementation from BDSAcceleratorComponent that builds the container volume.
  /// Load the geometry and place the components inside the container logical volume.
  virtual void BuildContainerLogicalVolume();

  G4double outerDiameter;
  G4String geometryFileName;
  G4String bMapFileName;
};

#endif
