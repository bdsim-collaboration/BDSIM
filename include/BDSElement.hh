#ifndef BDSELEMENT_H
#define BDSELEMENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh"

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
	     G4String fieldNameIn = "");
  virtual ~BDSElement(){;}
   
private:
  /// Private default constructor to force the use of the supplied one.
  BDSElement() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSElement& operator=(const BDSElement&) = delete;
  BDSElement(BDSElement&) = delete;
  /// @}
  
  /// This does the full construction.  Loads the external geometry and field if there is
  /// one.
  virtual void BuildContainerLogicalVolume();

  G4double outerDiameter;
  G4String geometryFileName;
  G4String fieldName;
};

#endif
