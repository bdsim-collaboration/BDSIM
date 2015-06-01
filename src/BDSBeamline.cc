#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "BDSDebug.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"

#include <iterator>
#include <ostream>
#include <utility>  // for std::pair
#include <vector>

BDSBeamline::BDSBeamline()
{;}

BDSBeamline::~BDSBeamline()
{
  BDSBeamlineIterator it = begin();
  for (; it != end(); ++it)
    {delete (*it);}
}

void BDSBeamline::PrintAllComponents(std::ostream& out) const
{
  BDSBeamlineIterator it = begin();
  for (; it != cend(); ++it)
    {out << *(it);}
}

std::ostream& operator<< (std::ostream& out, BDSBeamline const &beamline)
{
  out << "BDSBeamline with " << beamline.size() << " elements"<< G4endl
      << "Elements are: " << G4endl;
  beamline.PrintAllComponents(out);

  return out;
}

void BDSBeamline::AddComponent(BDSAcceleratorComponent* component)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " adding component" << G4endl;
#endif
  // test if component has finite length (different action in that case)

  // calculate the reference placement position
  // TBC!!!!
  G4ThreeVector referencePositionStart  = G4ThreeVector(0,0,0);
  G4ThreeVector referencePositionMiddle = G4ThreeVector(0,0,0);
  G4ThreeVector referencePositionEnd    = G4ThreeVector(0,0,0);

  // calculate the reference placement rotation
  // TBC!!!!
  G4RotationMatrix* referenceRotationStart  = new G4RotationMatrix();
  G4RotationMatrix* referenceRotationMiddle = new G4RotationMatrix();
  G4RotationMatrix* referenceRotationEnd    = new G4RotationMatrix();

  // add the placement offset
  G4double dx                  = component->GetXOffset();
  G4double dy                  = component->GetYOffset();
  G4ThreeVector displacement   = G4ThreeVector(dx,dy,0);
  G4ThreeVector positionStart  = referencePositionStart  + displacement;
  G4ThreeVector positionMiddle = referencePositionMiddle + displacement;
  G4ThreeVector positionEnd    = referencePositionEnd    + displacement;

  // add the tilt
  // TBC!!!!
  G4double tilt = component->GetTilt();
  G4RotationMatrix* rotationStart  = new G4RotationMatrix(*referenceRotationStart);
  rotationStart->rotateZ(tilt);
  G4RotationMatrix* rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
  rotationMiddle->rotateZ(tilt);
  G4RotationMatrix* rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);
  rotationEnd->rotateZ(tilt);

  // calculate the s position
  G4double sPositionStart  = 0;
  G4double sPositionMiddle = 0;
  G4double sPositionEnd    = 0;

  // construct beamline element
  BDSBeamlineElement* element = new BDSBeamlineElement(component,
						       positionStart,
						       positionMiddle,
						       positionEnd,
						       rotationStart,
						       rotationMiddle,
						       rotationEnd,
						       referencePositionStart,
						       referencePositionMiddle,
						       referencePositionEnd,
						       referenceRotationStart,
						       referenceRotationMiddle,
						       referenceRotationEnd,
						       sPositionStart,
						       sPositionMiddle,
						       sPositionEnd);
  
  // append it to the beam line
  beamline.push_back(element);
}
