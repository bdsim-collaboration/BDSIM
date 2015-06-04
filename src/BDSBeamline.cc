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
  BDSBeamlineElement* e = new BDSBeamlineElement(initialCoords,
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
						 0., 0., 0.);
  beamline.push_back(e);
  G4cout << e->GetPositionStart() << G4endl;
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
  G4cout << __METHOD_NAME__ << "adding component" << G4endl;
#endif

  // interrogate the item
  G4double      length     = component->GetChordLength();
  G4double      angle      = component->GetAngle();
  BDSTiltOffset tiltOffset = component->GetTiltOffset();
  G4bool hasFiniteLength   = BDS::IsFinite(length);
  G4bool hasFiniteAngle    = BDS::IsFinite(angle);
  G4bool hasFiniteTilt     = BDS::IsFinite(tiltOffset.GetTilt());
  G4bool hasFiniteOffset   = BDS::IsFinite(tiltOffset.GetXOffset()) || BDS::IsFinite(tiltOffset.GetYOffset());
  
#ifdef BDSDEBUG
  G4cout << "chord length         " << length     << " mm"         << G4endl;
  G4cout << "angle                " << angle      << " rad"        << G4endl;
  G4cout << "tilt offsetX offsetY " << tiltOffset << " rad mm mm " << G4endl;
  G4cout << "hasFiniteLength      " << hasFiniteLength             << G4endl;
  G4cout << "hasFiniteAngle       " << hasFiniteAngle              << G4endl;
  G4cout << "hasFiniteTilt        " << hasFiniteTilt               << G4endl;
  G4cout << "hasFiniteOffset      " << hasFiniteOffset             << G4endl;
#endif
  
  // calculate the reference placement rotation
  // rotations are done first as they're required to transform the spatial displacements
  // copy the rotation matrix (cumulative along line) from end of last component
  // can use beamline.back here as we'll always have one element added in the constructor
  G4RotationMatrix* previousReferenceRotationEnd = beamline.back()->GetReferenceRotationEnd();
  G4RotationMatrix* referenceRotationStart       = new G4RotationMatrix(*previousReferenceRotationEnd);
  G4RotationMatrix* referenceRotationMiddle      = new G4RotationMatrix(*referenceRotationStart);
  G4RotationMatrix* referenceRotationEnd         = new G4RotationMatrix(*referenceRotationStart);
  // if the component induces an angle in the reference trajectory, rotate the mid and end point
  // rotation matrices appropriately
  if (hasFiniteAngle)
    {
      referenceRotationMiddle->rotateY(component->GetAngle() * 0.5); // middle rotated by half angle in x,z plane
      referenceRotationEnd->rotateY(component->GetAngle());          // end rotated by full angle in x,z plane
    }

  // add the tilt to the rotation matrices (around z axis)
  G4RotationMatrix* rotationStart, *rotationMiddle, *rotationEnd;
  if (hasFiniteTilt)
    {
      G4double tilt = tiltOffset.GetTilt();
      rotationStart  = new G4RotationMatrix(*referenceRotationStart);
      rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
      rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);
      rotationStart->rotateZ(tilt);
      rotationMiddle->rotateZ(tilt);
      rotationEnd->rotateZ(tilt);
    }
  else
    {
      rotationStart  = new G4RotationMatrix(*referenceRotationStart);
      rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
      rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);
    }
  
  // calculate the reference placement position
  G4ThreeVector previousReferencePositionEnd = beamline.back()->GetReferencePositionEnd();
  G4ThreeVector referencePositionStart, referencePositionMiddle, referencePositionEnd;
  if (hasFiniteLength)
    {
      referencePositionStart  = previousReferencePositionEnd;
      referencePositionMiddle = referencePositionStart + G4ThreeVector(0, 0, 0.5 * length).transform(*referenceRotationMiddle);
      // remember the end position is the chord length along the half angle, not the full angle
      // the particle achieves the full angle though by the end position.
      referencePositionEnd    = referencePositionStart + G4ThreeVector(0, 0, length).transform(*referenceRotationMiddle);
    }
  else
    {
      // element has no finite size so all positions are previous end position
      // likely this is a transform3d or similar - but not hard coded just for transform3d
      referencePositionStart  = previousReferencePositionEnd;
      referencePositionMiddle = previousReferencePositionEnd;
      referencePositionEnd    = previousReferencePositionEnd;
    }
  
  // add the placement offset
  G4ThreeVector positionStart, positionMiddle, positionEnd;
  if (hasFiniteOffset)
    {
      G4double dx                  = tiltOffset.GetXOffset();
      G4double dy                  = tiltOffset.GetYOffset();
      // note the displacement is applied in the accelerator x and y frame so use
      // the reference rotation rather than the one with tilt already applied
      G4ThreeVector displacement   = G4ThreeVector(dx,dy,0).transform(*referenceRotationMiddle);
      positionStart  = referencePositionStart  + displacement;
      positionMiddle = referencePositionMiddle + displacement;
      positionEnd    = referencePositionEnd    + displacement;
    }
  else
    {
      positionStart  = referencePositionStart;
      positionMiddle = referencePositionMiddle;
      positionEnd    = referencePositionEnd;
    }
  
  // calculate the s position
  G4double      previousSPositionEnd = beamline.back()->GetSPositionEnd();
  G4double      arcLength            = component->GetArcLength();
  G4double sPositionStart, sPositionMiddle, sPositionEnd;
  sPositionStart  = previousSPositionEnd;
  sPositionMiddle = previousSPositionEnd + 0.5 * arcLength;
  sPositionEnd    = previousSPositionEnd + arcLength;


#ifdef BDSDEBUG
  // feedback about calculated coordinates
  G4cout << "calculated coordinates in mm and rad are " << G4endl;
  G4cout << "reference position start:  " << referencePositionStart   << G4endl;
  G4cout << "reference position middle: " << referencePositionMiddle  << G4endl;
  G4cout << "reference position end:    " << referencePositionEnd     << G4endl;
  G4cout << "reference rotation start:  " << *referenceRotationStart;
  G4cout << "reference rotation middle: " << *referenceRotationMiddle;
  G4cout << "reference rotation end:    " << *referenceRotationEnd;
  G4cout << "position start:            " << positionStart            << G4endl;
  G4cout << "position middle:           " << positionMiddle           << G4endl;
  G4cout << "position end:              " << positionEnd              << G4endl;
  G4cout << "rotation start:            " << *rotationStart;
  G4cout << "rotation middle:           " << *rotationMiddle;
  G4cout << "rotation end:              " << *rotationEnd;
#endif

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
