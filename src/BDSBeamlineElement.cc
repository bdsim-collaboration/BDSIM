#include "BDSBeamlineElement.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <ostream>

BDSBeamlineElement::BDSBeamlineElement(BDSAcceleratorComponent* component,
				       G4ThreeVector            positionStartIn,
				       G4ThreeVector            positionMiddleIn,
				       G4ThreeVector            positionEndIn,
				       G4RotationMatrix*        rotationStartIn,
				       G4RotationMatrix*        rotationMiddleIn,
				       G4RotationMatrix*        rotationEndIn,
				       G4double                 sPositionStartIn,
				       G4double                 sPositionMiddleIn,
				       G4double                 sPositionEndIn):
  BDSGeometryComponent::BDSGeometryComponent(*(BDSGeometryComponent*)component),
  positionStart(positionStartIn), positionMiddle(positionMiddleIn), positionEnd(positionEndIn),
  rotationStart(rotationStartIn), rotationMiddle(rotationMiddleIn), rotationEnd(rotationEndIn),
  sPositionStart(sPositionStartIn), sPositionMiddle(sPositionMiddleIn), sPositionEnd(sPositionEndIn)
{
  name = component->GetName();
}

std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &e)
{
  out << "Beamline element: " << e.component->GetName() << G4endl;
  out << "Start, middle & end position: "
      << e.GetPositionStart() << " " << e.GetPositionMiddle() << " " << e.GetPositionEnd() << G4endl;
  out << "Start, middle & end rotation: "
    //<< e.GetRotationStart() << " " << e.GetRotationMiddle() << " " << e.GetRotationEnd() << G4endl;
      << "Start, middle 7 end s position: "
      << e.GetSPositionStart() << " " << e.GetSPositionMiddle() << " " << e.GetSPositionEnd() << G4endl;

  return out;
}
