#include "BDSBeamlineElement.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"

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
				       G4ThreeVector            xAxisReferenceStartIn,
				       G4ThreeVector            yAxisReferenceStartIn,
				       G4ThreeVector            zAxisReferenceStartIn,
				       G4ThreeVector            xAxisReferenceMiddleIn,
				       G4ThreeVector            yAxisReferenceMiddleIn,
				       G4ThreeVector            zAxisReferenceMiddleIn,
				       G4ThreeVector            xAxisReferenceEndIn,
				       G4ThreeVector            yAxisReferenceEndIn,
				       G4ThreeVector            zAxisReferenceEndIn):
  BDSGeometryComponent(*((BDSGeometryComponent*)componentIn)),
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
  xAxisReferenceStart(xAxisReferenceStartIn),
  yAxisReferenceStart(yAxisReferenceStartIn),
  zAxisReferenceStart(zAxisReferenceStartIn),
  xAxisReferenceMiddle(xAxisReferenceMiddleIn),
  yAxisReferenceMiddle(yAxisReferenceMiddleIn),
  zAxisReferenceMiddle(zAxisReferenceMiddleIn),
  xAxisReferenceEnd(xAxisReferenceEndIn),
  yAxisReferenceEnd(yAxisReferenceEndIn),
  zAxisReferenceEnd(zAxisReferenceEndIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__;
  if (componentIn)
    {G4cout << componentIn->GetName();}
  else
    {G4cerr << "WARNING - supplied component is in valid!" << G4endl;}
  G4cout << G4endl;
#endif
}

BDSBeamlineElement::~BDSBeamlineElement()
{
  delete component;
  delete rotationStart;
  delete rotationMiddle;
  delete rotationEnd;
  delete referenceRotationStart;
  delete referenceRotationMiddle;
  delete referenceRotationEnd;  
}

std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &e)
{
  out << "Beamline element: " << e.component->GetName() << G4endl;
  out << "Start, middle & end position: "
      << e.GetPositionStart()  << " " << e.GetPositionMiddle()  << " " << e.GetPositionEnd()  << G4endl
      << "Start, middle & end rotation: "
      << *(e.GetRotationStart())  << " " << *(e.GetRotationMiddle())  << " " << *(e.GetRotationEnd())  << G4endl
      << "Start, middle & end s position: "
      << e.GetSPositionStart() << " " << e.GetSPositionMiddle() << " " << e.GetSPositionEnd() << G4endl
      << "X axis at the start, middle & end: "
      << e.GetXAxisReferenceStart() << " " << e.GetXAxisReferenceMiddle() << " " << e.GetXAxisReferenceEnd() << G4endl
      << "Y axis at the start, middle & end: "
      << e.GetYAxisReferenceStart() << " " << e.GetYAxisReferenceMiddle() << " " << e.GetYAxisReferenceEnd() << G4endl
      << "Z axis at the start, middle & end: "
      << e.GetZAxisReferenceStart() << " " << e.GetZAxisReferenceMiddle() << " " << e.GetZAxisReferenceEnd() << G4endl;

  return out;
}
