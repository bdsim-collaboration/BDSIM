#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "BDSDebug.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSSampler.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include <iterator>
#include <ostream>
#include <utility>  // for std::pair
#include <vector>

BDSBeamline::BDSBeamline()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4ThreeVector     zeroPos = G4ThreeVector(0,0,0);
  G4RotationMatrix* zeroRot = new G4RotationMatrix();
  BDSSampler* initialCoords = new BDSSampler("initial_coordinates",1e-4);
  initialCoords->Initialise(); // builds and assigns volumes
  beamline.push_back( new BDSBeamlineElement(initialCoords,
					     zeroPos,
					     zeroPos,
					     zeroPos,
					     zeroRot,
					     zeroRot,
					     zeroRot,
					     zeroPos,
					     zeroPos,
					     zeroPos,
					     zeroRot,
					     zeroRot,
					     zeroRot,
					     0., 0., 0.)
		      );
}

BDSBeamline::~BDSBeamline()
{
  BDSBeamlineIterator it = begin();
  for (; it != end(); ++it)
    {delete (*it);}

  // and delete the null one at the beginning
  delete beamline[0];
}

void BDSBeamline::PrintAllComponents(std::ostream& out) const
{
  BDSBeamlineIterator it = begin();
  for (; it != end(); ++it)
    {out << *(it);}
}

std::ostream& operator<< (std::ostream& out, BDSBeamline const &bl)
{
  out << "BDSBeamline with " << bl.size() << " elements"<< G4endl
      << "Elements are: " << G4endl;
  bl.PrintAllComponents(out);

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

BDSBeamlineElement* BDSBeamline::front() const
{
  if (beamline.size() == 1)
    {
      G4cerr << __METHOD_NAME__ << "empty beamline" << G4endl;
      exit(1);
    }
  return beamline.front();
}

BDSBeamlineElement* BDSBeamline::back() const
{
  if (beamline.size() == 1)
    {
      G4cerr << __METHOD_NAME__ << "empty beamline" << G4endl;
      exit(1);
    }
  return beamline.back();
}
