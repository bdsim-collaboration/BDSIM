#include "UDipole.hh"
#include "UDipoleConstructor.hh"

#include "BDSAcceleratorComponent.hh"
#include "parser/element.h" // for GMAD::Element

#include "CLHEP/Units/SystemOfUnits.h"

#include "G4Types.hh" // for G4String etc

BDSAcceleratorComponent* UDipoleConstructor::Construct(GMAD::Element const* element,
						       GMAD::Element const* /*prevElement*/,
						       GMAD::Element const* /*nextElement*/,
						       G4double /*currentArcLength*/,
						       G4double /*brhoIn*/,
						       G4double /*beta0In*/)
{
  G4String params = G4String(element->userParameters);
  
  // Here we pull out the information we want from the parser element and construct
  // one of our components and return it. BDSIM will delete this at the end.
  // If you don't need to use all the parameters, use an /*inline*/ comment to
  // avoid a compiler warning about an unused variable.
  BDSAcceleratorComponent* dipole = new UDipole(element->name,
						element->B*CLHEP::tesla,
						params);
  return dipole;
}
