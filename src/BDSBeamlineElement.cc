#include "BDSBeamlineElement.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <ostream>

BDSBeamlineElement::BDSBeamlineElement(BDSAcceleratorComponent* componentIn,
				       G4ThreeVector            positionStartIn,
				       G4ThreeVector            positionMiddleIn,
				       G4ThreeVector            positionEndIn,
				       G4RotationMatrix*        rotationStartIn,
				       G4RotationMatrix*        rotationMiddleIn,
				       G4RotationMatrix*        rotationEndIn,
				       G4ThreeVector            referencePositionStartIn,
				       G4ThreeVector            referencePositionMiddleIn,
				       G4ThreeVector            referencePositionEndIn,
				       G4RotationMatrix*        referenceRotationStartIn,
				       G4RotationMatrix*        referenceRotationMiddleIn,
				       G4RotationMatrix*        referenceRotationEndIn,
				       G4double                 sPositionStartIn,
				       G4double                 sPositionMiddleIn,
				       G4double                 sPositionEndIn,
				       BDSSamplerType           samplerTypeIn,
				       G4String                 samplerNameIn,
				       G4int                    indexIn):
  component(componentIn),
  positionStart(positionStartIn), positionMiddle(positionMiddleIn), positionEnd(positionEndIn),
  rotationStart(rotationStartIn), rotationMiddle(rotationMiddleIn), rotationEnd(rotationEndIn),
  referencePositionStart(referencePositionStartIn),
  referencePositionMiddle(referencePositionMiddleIn),
  referencePositionEnd(referencePositionEndIn),
  referenceRotationStart(referenceRotationStartIn),
  referenceRotationMiddle(referenceRotationMiddleIn),
  referenceRotationEnd(referenceRotationEndIn),
  sPositionStart(sPositionStartIn), sPositionMiddle(sPositionMiddleIn), sPositionEnd(sPositionEndIn),
  samplerType(samplerTypeIn),
  samplerName(samplerNameIn),
  samplerPlacementTransform(nullptr),
  index(indexIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__;
  if (componentIn)
    {G4cout << componentIn->GetName();}
  else
    {G4cerr << "WARNING - supplied component is invalid!" << G4endl;}
  G4cout << G4endl;
#endif

  componentIn->IncrementCopyNumber(); // increase copy number (starts at -1)
  copyNumber = componentIn->GetCopyNumber();
  /// placement name (starting at 0)
  placementName = componentIn->GetName() + "_" + std::to_string(copyNumber);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "unique placement name: \"" << placementName << "_pv\"" << G4endl;
#endif

  // create the placement transform from supplied rotation matrices and vector
  placementTransform        = new G4Transform3D(*rotationMiddle, positionMiddle);
  readOutPlacementTransform = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);

  // calculate sampler central position slightly away from end position of element.
  if (samplerType == BDSSamplerType::plane)
    {
      G4ThreeVector dZLocal = G4ThreeVector(0,0,1); // initialise with local unit z
      // offset from end face as overlapping faces produce weird results - no hits etc
      // 1um seemed ok, but then ~20 / 10k would appear wrong - last step in world volume
      // instead of correct place - should be impossible.  Also had nullptr for track->GetVolume()
      // empirically found good results with 2um (at least 1.5um)
      // back off to avoid overlaps
      dZLocal *= 0.5*BDSSamplerPlane::ChordLength() + 2*CLHEP::um; 
      dZLocal.transform(*referenceRotationEnd);
      G4ThreeVector samplerPosition = referencePositionEnd - dZLocal;
      samplerPlacementTransform = new G4Transform3D(*referenceRotationEnd, samplerPosition);
    }
  else if (samplerType == BDSSamplerType::cylinder)
    {samplerPlacementTransform = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);}
}

BDSBeamlineElement::~BDSBeamlineElement()
{
  delete rotationStart;
  delete rotationMiddle;
  delete rotationEnd;
  delete referenceRotationStart;
  delete referenceRotationMiddle;
  delete referenceRotationEnd;
  delete placementTransform;
  delete readOutPlacementTransform;
  delete samplerPlacementTransform;
}

std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &e)
{
  out << "Beamline element: " << e.component->GetName() << G4endl;
  out << "Start, middle & end position: "
      << e.GetPositionStart()  << " " << e.GetPositionMiddle()  << " " << e.GetPositionEnd()  << G4endl
      << "Start, middle & end rotation: "
      << *(e.GetRotationStart())  << " " << *(e.GetRotationMiddle())  << " " << *(e.GetRotationEnd())  << G4endl
      << "Start, middle & end s position: "
      << e.GetSPositionStart() << " " << e.GetSPositionMiddle() << " " << e.GetSPositionEnd() << G4endl;

  return out;
}
