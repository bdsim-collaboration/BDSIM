#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSDebug.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSLine.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"
#include "BDSUtilities.hh"

#include <algorithm>
#include <iterator>
#include <ostream>
#include <utility>  // for std::pair
#include <vector>

BDSBeamline::BDSBeamline(G4ThreeVector     initialGlobalPosition,
			 G4RotationMatrix* initialGlobalRotation)
{
  // initialise extents
  totalChordLength      = 0;
  totalArcLength        = 0;
  maximumExtentPositive = G4ThreeVector(0,0,0);
  maximumExtentNegative = G4ThreeVector(0,0,0);
  
  // initial rotation matrix
  if (initialGlobalRotation) // default is null
    {previousReferenceRotationEnd = initialGlobalRotation;}
  else
    {previousReferenceRotationEnd = new G4RotationMatrix();}

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "with initial position and rotation" << G4endl;
  G4cout << "Initial position: " << initialGlobalPosition << G4endl;
  G4cout << "Initial rotation: " << *previousReferenceRotationEnd << G4endl;
#endif

  // initial position
  previousReferencePositionEnd = initialGlobalPosition;

  // initial s coordinate
  previousSPositionEnd = 0; 
}

BDSBeamline::~BDSBeamline()
{
  for (iterator it = begin(); it != end(); ++it)
    {delete (*it);}
  // special case, if empty then previousReferenceRotationEnd is not used in the first element
  if (size()==0)
    {delete previousReferenceRotationEnd;}
  // components map goes out of scope - elements are already deleted - no need to
  // explicitly delete
}

void BDSBeamline::PrintAllComponents(std::ostream& out) const
{
  for (const_iterator it = begin(); it != end(); ++it)
    {out << *(it);}
}

void BDSBeamline::PrintMemoryConsumption() const
{
  G4cout << __METHOD_NAME__ << "container size: " << sizeof(beamline) << G4endl;
  G4cout << __METHOD_NAME__ << "beamline element cumulative size: " << sizeof(BDSBeamlineElement) * beamline.size() << G4endl;
  G4cout << __METHOD_NAME__ << "full usage including components:  " << (sizeof(BDSBeamlineElement) + sizeof(BDSAcceleratorComponent)) * beamline.size() << G4endl;
}

std::ostream& operator<< (std::ostream& out, BDSBeamline const &bl)
{
  out << "BDSBeamline with " << bl.size() << " elements"<< G4endl
      << "Elements are: " << G4endl;
  bl.PrintAllComponents(out);
  out << G4endl;
  out << "Total arc length:   " << bl.totalArcLength   << " mm" << G4endl;
  out << "Total chord length: " << bl.totalChordLength << " mm" << G4endl;

  return out;
}

std::vector<BDSBeamlineElement*> BDSBeamline::AddComponent(BDSAcceleratorComponent* component, BDSTiltOffset* tiltOffset)
{
  std::vector<BDSBeamlineElement*> addedComponents;
  BDSBeamlineElement* element = nullptr;
  // if default nullptr is supplied as tilt offset use a default 0,0,0,0 one
  if (!tiltOffset) {tiltOffset  = new BDSTiltOffset();}
  
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      for (BDSLine::iterator i = line->begin(); i != line->end(); ++i)
	{
	  element = AddSingleComponent(*i, tiltOffset);
	  if (element) addedComponents.push_back(element);
	}
    }
  else
    {
      element = AddSingleComponent(component, tiltOffset);
      if (element) addedComponents.push_back(element);
    }
  // free memory - as once the rotations are calculated, this is no longer needed
  delete tiltOffset;
  
  return addedComponents;
}

BDSBeamlineElement* BDSBeamline::AddSingleComponent(BDSAcceleratorComponent* component, BDSTiltOffset* tiltOffset)
{
#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "adding component to beamline and calculating coordinates" << G4endl;
  G4cout << "component name:      " << component->GetName() << G4endl;
#endif
  
  // Test if it's a BDSTransform3D instance - this is a unique component that requires
  // rotation in all dimensions and can skip normal addition as isn't a real volume
  // that can be placed.  Apply the transform and skip the rest of this function by returning
  // This modifies the "end" coordinates, rotation and axes of the last element in the beamline
  if (BDSTransform3D* transform = dynamic_cast<BDSTransform3D*>(component))
    {
      ApplyTransform3D(transform);
      return nullptr;
    }

  // if it's not a transform3d instance, continue as normal
  // interrogate the item
  G4double      length   = component->GetChordLength();
  G4double      angle    = component->GetAngle();
  G4bool hasFiniteLength = BDS::IsFinite(length);
  G4bool hasFiniteAngle  = BDS::IsFinite(angle);
  G4bool hasFiniteTilt   = BDS::IsFinite(tiltOffset->GetTilt());
  G4bool hasFiniteOffset = BDS::IsFinite(tiltOffset->GetXOffset()) || BDS::IsFinite(tiltOffset->GetYOffset());
  G4ThreeVector offset   = G4ThreeVector(tiltOffset->GetXOffset(), tiltOffset->GetYOffset(), 0);
  G4ThreeVector eP       = component->GetExtentPositive() + offset;
  G4ThreeVector eN       = component->GetExtentNegative() + offset;
  G4ThreeVector placementOffset   = component->GetPlacementOffset();
  G4bool hasFinitePlacementOffset = BDS::IsFinite(placementOffset);
  
#ifdef BDSDEBUG
  G4cout << "chord length                " << length      << " mm"         << G4endl;
  G4cout << "angle                       " << angle       << " rad"        << G4endl;
  G4cout << "tilt offsetX offsetY        " << *tiltOffset << " rad mm mm " << G4endl;
  G4cout << "has finite length           " << hasFiniteLength              << G4endl;
  G4cout << "has finite angle            " << hasFiniteAngle               << G4endl;
  G4cout << "has finite tilt             " << hasFiniteTilt                << G4endl;
  G4cout << "has finite offset           " << hasFiniteOffset              << G4endl;
  G4cout << "extent positive             " << eP                           << G4endl;
  G4cout << "extent negative             " << eN                           << G4endl;
  G4cout << "object placement offset     " << placementOffset              << G4endl;
  G4cout << "has finite placement offset " << hasFinitePlacementOffset     << G4endl;
#endif
  
  // Calculate the reference placement rotation
  // rotations are done first as they're required to transform the spatial displacements.
  // if not the first element in the beamline, copy the rotation matrix (cumulative along line)
  // from end of last component, else use initial rotation matrix (no copy to prevent memory leak)
  G4RotationMatrix* referenceRotationStart;
  if (empty())
    {referenceRotationStart = previousReferenceRotationEnd;}
  else
    {
      previousReferenceRotationEnd = back()->GetReferenceRotationEnd();
      referenceRotationStart  = new G4RotationMatrix(*previousReferenceRotationEnd);
    }

  G4RotationMatrix* referenceRotationMiddle = new G4RotationMatrix(*referenceRotationStart);
  G4RotationMatrix* referenceRotationEnd    = new G4RotationMatrix(*referenceRotationStart);
  
  // if the component induces an angle in the reference trajectory, rotate the mid and end point
  // rotation matrices appropriately
  if (hasFiniteAngle)
    {
      G4double angle = component->GetAngle();
      // remember our definition of angle - +ve angle bends in -ve x direction in right
      // handed coordinate system
      // rotate about cumulative local y axis of beamline
      // middle rotated by half angle in local x,z plane
      G4ThreeVector unitY = G4ThreeVector(0,1,0);
      referenceRotationMiddle->rotate(angle*0.5, unitY.transform(*previousReferenceRotationEnd));
      // end rotated by full angle in local x,z plane
      G4ThreeVector unitYEnd = G4ThreeVector(0,1,0);
      referenceRotationEnd->rotate(angle, unitYEnd.transform(*previousReferenceRotationEnd));
    }
  
  // add the tilt to the rotation matrices (around z axis)
  G4RotationMatrix* rotationStart, *rotationMiddle, *rotationEnd;
  if (hasFiniteTilt  && !hasFiniteAngle)
    {
      G4double tilt = tiltOffset->GetTilt();
      rotationStart  = new G4RotationMatrix(*referenceRotationStart);
      rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
      rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);

      // transform a unit z vector with the rotation matrices to get the local axes
      // of rotation to apply the tilt.
      G4ThreeVector unitZ = G4ThreeVector(0,0,1);
      rotationStart ->rotate(tilt, unitZ.transform(*referenceRotationStart));
      unitZ = G4ThreeVector(0,0,1);
      rotationMiddle->rotate(tilt, unitZ.transform(*referenceRotationMiddle));
      unitZ = G4ThreeVector(0,0,1);
      rotationEnd   ->rotate(tilt, unitZ.transform(*referenceRotationEnd));
    }
  else
    {
      // note, don't apply tilt if the object has finite angle as this will cause
      // geometry overlaps
      if (hasFiniteAngle && hasFiniteTilt)
	{
	  G4String name = component->GetName();
	  G4cout << __METHOD_NAME__ << "WARNING - element has tilt, but this will cause geometry"
		 << " overlaps: " << name << " - omitting tilt" << G4endl;
	}
      rotationStart  = new G4RotationMatrix(*referenceRotationStart);
      rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
      rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);
    }
  
  // calculate the reference placement position
  // if not the first item in the beamline, get the reference trajectory global position
  // at the end of the previous element
  if (!empty())
    {previousReferencePositionEnd = back()->GetReferencePositionEnd();}
  
  G4ThreeVector referencePositionStart, referencePositionMiddle, referencePositionEnd;
  if (hasFiniteLength)
    {
      referencePositionStart  = previousReferencePositionEnd;
      // calculate delta to mid point
      G4ThreeVector md = G4ThreeVector(0, 0, 0.5 * length);
      md.transform(*referenceRotationMiddle);
      referencePositionMiddle = referencePositionStart + md;
      // remember the end position is the chord length along the half angle, not the full angle
      // the particle achieves the full angle though by the end position.
      G4ThreeVector delta = G4ThreeVector(0, 0, length).transform(*referenceRotationMiddle);
      referencePositionEnd = referencePositionStart + delta;
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
  if (hasFiniteOffset || hasFinitePlacementOffset)
    {
      if (hasFiniteOffset && hasFiniteAngle) // do not allow x offsets for bends as this will cause overlaps
	{
	  G4String name = component->GetName();
	  G4cout << __METHOD_NAME__ << "WARNING - element has x offset, but this will cause geometry"
		 << " overlaps: " << name << " - omitting x offset" << G4endl;
	  offset.setX(0.0);
	}
      // note the displacement is applied in the accelerator x and y frame so use
      // the reference rotation rather than the one with tilt already applied
      G4ThreeVector total = offset + placementOffset;
      G4ThreeVector displacement   = total.transform(*referenceRotationMiddle);
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
  // if not the first element in the beamline, get the s position at the end of the previous element
  if (!empty())
    {previousSPositionEnd = back()->GetSPositionEnd();}
  
  G4double arcLength   = component->GetArcLength();
  G4double chordLength = component->GetChordLength();

  // integrate lengths
  totalChordLength += chordLength;
  totalArcLength   += arcLength;

  // advance s coordinate
  G4double sPositionStart, sPositionMiddle, sPositionEnd;
  sPositionStart  = previousSPositionEnd;
  sPositionMiddle = previousSPositionEnd + 0.5 * arcLength;
  sPositionEnd    = previousSPositionEnd + arcLength;

  // update the global constants
  BDSGlobalConstants::Instance()->SetSMax(sPositionEnd);

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

  // calculate extents for world size determination
  UpdateExtents(element);
  
  // append it to the beam line
  beamline.push_back(element);

  // register the s position at the end for curvlinear transform
  sEnd.push_back(sPositionEnd);

  // register it by name
  RegisterElement(element);

#ifdef BDSDEBUG
  G4cout << *element;
  G4cout << __METHOD_NAME__ << "component added" << G4endl;
#endif
  return element;
}

void BDSBeamline::ApplyTransform3D(BDSTransform3D* component)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "- as it's a transform3d instance" << G4endl;
#endif
  // interrogate component
  G4double dx     = component->GetDX();
  G4double dy     = component->GetDY();
  G4double dz     = component->GetDZ();
  G4double dTheta = component->GetDTheta();
  G4double dPsi   = component->GetDPsi();
  G4double dPhi   = component->GetDPhi();
  
  // debug feedback
#ifdef BDSDEBUG
  G4cout << "dx     " << dx     << G4endl;
  G4cout << "dy     " << dy     << G4endl;
  G4cout << "dz     " << dz     << G4endl;
  G4cout << "dTheta " << dTheta << G4endl;
  G4cout << "dPsi   " << dPsi   << G4endl;
  G4cout << "dPhi   " << dPhi   << G4endl;
#endif

  // test validity for overlaps
  if (dz < 0)
    {
      G4cerr << __METHOD_NAME__ << "Problemm with Transform3d: " << component->GetName() << G4endl;
      G4cerr << __METHOD_NAME__ << "dz = " << dz << " < 0 -> will overlap previous element" << G4endl;
    } 

  // if not the first element in the beamline, get information from the end of the
  // last element in the beamline
  if (!empty())
    {
      BDSBeamlineElement* last = back();
      previousReferenceRotationEnd = last->GetReferenceRotationEnd();
      previousReferencePositionEnd = last->GetReferencePositionEnd();
    }

  // apply position
  // transform the local dx,dy,dz displacement into the global frame then apply
  G4ThreeVector delta = G4ThreeVector(dx,dy,dz).transform(*previousReferenceRotationEnd);
  previousReferencePositionEnd = previousReferencePositionEnd + G4ThreeVector(dx,dy,dz);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "existing end rotation matrix:" << *previousReferenceRotationEnd << G4endl;
#endif
  
  // apply rotation
  // euler angles must be applied in sequence about the cumulatively rotated axes
  // use unit vectors that are transformed to the current cumulative rotation of the beamline
  // as rotation axes for each angle.
  G4ThreeVector unitZ = G4ThreeVector(0,0,1);
  // transform to the current local z axis at the end of the beamline
  unitZ.transform(*previousReferenceRotationEnd);
  // apply the phi (alpha) euler angle about the z axis (step1)
  previousReferenceRotationEnd->rotate(dPhi, unitZ);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "after apply Phi:" << *previousReferenceRotationEnd << G4endl;
#endif
  // apply the theta (beta) euler angle about the N or x' axis (step 2)
  // transform a unit x to axes rotated by phi (called N or x')
  G4ThreeVector unitX = G4ThreeVector(1,0,0);
  unitX.transform(*previousReferenceRotationEnd);
  // rotate about N by dTheta
  previousReferenceRotationEnd->rotate(dTheta, unitX);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "after apply Phi & Theta:" << *previousReferenceRotationEnd << G4endl;
#endif
  // apply the psi (gamma) euler angle about the Z or z'' axis
  G4ThreeVector unitZPP = G4ThreeVector(0,0,1); //Z Prime Prime
  // get unit z to local axes already rotated by phi and theta hence zpp name
  unitZPP.transform(*previousReferenceRotationEnd);
  // rotate by psi about zpp
  previousReferenceRotationEnd->rotate(dPsi, unitZPP);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "new end rotation matrix:" << *previousReferenceRotationEnd << G4endl;
#endif
}

void BDSBeamline::AddBeamlineElement(BDSBeamlineElement* element)
{
  // update world extent for this beam line
  UpdateExtents(element);
  
  // append it to the beam line
  beamline.push_back(element);

  // register it by name
  RegisterElement(element);

  // no need to update any internal variables - that's done by
  // AddSingleComponent()
}

G4ThreeVector BDSBeamline::GetMaximumExtentAbsolute() const
{
  G4ThreeVector mEA;
  for (int i=0; i<3; i++)
    {mEA[i] = std::max(std::abs(maximumExtentPositive[i]), std::abs(maximumExtentNegative[i]));}
  return mEA;
}

G4Transform3D BDSBeamline::GetGlobalEuclideanTransform(G4double s, G4double x, G4double y)
{
  // check if s is in the range of the beamline
  if (s > totalArcLength)
    {
      G4cout << __METHOD_NAME__
	     << "s position \"" << s << "\" is beyond length of accelerator" << G4endl;
      G4cout << "Returning 0 transform" << G4endl;
      return G4Transform3D();
    }

  // find element that s position belongs to
  auto lower = std::lower_bound(sEnd.begin(), sEnd.end(), s);
  G4int index = lower - sEnd.begin(); // subtract iterators to get index
  BDSBeamlineElement* element = beamline[index];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "S position requested: " << s     << G4endl;
  G4cout << "Index:                " << index << G4endl;
  G4cout << "Element: " << *element << G4endl;
#endif

  G4double dx = 0;
  // G4double dy = 0; // currently magnets can only bend in local x so avoid extra calculation

  // difference from centre of element to point in local coords)
  // difference in s from centre, normalised to arcLengh and scaled to chordLength
  // as s is really arc length but we must place effectively in chord length coordinates
  BDSAcceleratorComponent* component = element->GetAcceleratorComponent();
  G4double arcLength   = component->GetArcLength();
  G4double chordLength = component->GetChordLength();
  G4double dS          = s - element->GetSPositionMiddle();
  G4double localZ      = dS * (chordLength / arcLength);
  G4double angle       = component->GetAngle();
  G4RotationMatrix rotation; // will be interpolated rotation
  G4RotationMatrix* rotMiddle = element->GetReferenceRotationMiddle();
  // find offset of point from centre of volume - 2 methods
  if (BDS::IsFinite(angle))
    {
      // finite bend angle - interpolate position and angle along arc due to change in angle
      // local unit z at start of element
      G4ThreeVector localUnitY = G4ThreeVector(0,1,0);
      localUnitY.transform(*(element->GetReferenceRotationStart()));
      // linearly interpolate angle -> angle * (s from beginning into component)/arcLength
      G4double partialAngle = angle * std::fabs(( (0.5*arcLength + dS) / arcLength));
      rotation = G4RotationMatrix(*(element->GetReferenceRotationStart())); // start rotation
      rotation.rotate(partialAngle, localUnitY); // rotate it by the partial angle about local Y
      dx = localZ*tan(partialAngle); // calculate difference of arc from chord at that point
    }
  else
    {rotation = G4RotationMatrix(*rotMiddle);}

  // note, magnets only bend in local x so no need to add dy as always 0
  G4ThreeVector dLocal    = G4ThreeVector(x + dx, y /*+ dy*/, localZ);
#ifdef BDSDEBUG
  G4cout << "Local offset from middle: " << dLocal << G4endl;
#endif
  // note, rotation middle is also the same as the coordinate frame of the g4 solid
  G4ThreeVector globalPos = element->GetReferencePositionMiddle() + dLocal.transform(*rotMiddle);
  // construct transform3d from global position and rotation matrix
  G4Transform3D result    = G4Transform3D(rotation, globalPos);
  
#ifdef BDSDEBUG
  G4cout << "Global offset from middle: " << dLocal    << G4endl;
  G4cout << "Resultant global position: " << globalPos << G4endl;
#endif
  return result;
}

void BDSBeamline::RegisterElement(BDSBeamlineElement* element)
{
  // check if base name already registered (can be single component placed multiple times)
  std::map<G4String, BDSBeamlineElement*>::iterator search = components.find(element->GetName());
  if (search == components.end())
    {
      // not registered
      components[element->GetName()] = element;
    }
  // else - already registered - pass it by
  /*
  search = components.find(element->GetPlacementName());
  if (search == component.edn())
    {
      //not registered
      components[element->GetPlacementName()] = element;
      }*/ //
  // UNCOMMENT WHEN MERGED WITH TUNNEL BR for better placement names
  // else - already registered - pass it by
}

BDSBeamlineElement* BDSBeamline::GetElement(G4String name)
{
  std::map<G4String, BDSBeamlineElement*>::iterator search = components.find(name);
  if (search == components.end())
    {//wasn't found
      return nullptr;
    }
  else
    {return search->second;}
}


void BDSBeamline::UpdateExtents(BDSBeamlineElement* element)
{
  // calculate extents for world size determination
  // project size in global coordinates
  G4ThreeVector     referencePositionStart = element->GetReferencePositionStart();
  G4RotationMatrix* referenceRotationStart = element->GetReferenceRotationStart();
  G4ThreeVector     referencePositionEnd   = element->GetReferencePositionEnd();
  G4RotationMatrix* referenceRotationEnd   = element->GetReferenceRotationEnd();
  BDSAcceleratorComponent* component       = element->GetAcceleratorComponent();
  G4ThreeVector eP                         = component->GetExtentPositive();
  eP.setZ(0); // we get the z position from the start point, so only need the transverse bits
  G4ThreeVector eN                         = component->GetExtentNegative();
  eN.setZ(0); // we get the z position from the start point, so only need the transverse bits
  G4ThreeVector ePStart                    = G4ThreeVector(eP).transform(*referenceRotationStart);
  G4ThreeVector eNStart                    = G4ThreeVector(eN).transform(*referenceRotationStart);
  G4ThreeVector ePEnd                      = G4ThreeVector(eP).transform(*referenceRotationEnd);
  G4ThreeVector eNEnd                      = G4ThreeVector(eN).transform(*referenceRotationEnd);
  G4ThreeVector extentposStart             = referencePositionStart + ePStart;
  G4ThreeVector extentnegStart             = referencePositionStart + eNStart;
  G4ThreeVector extentposEnd               = referencePositionEnd   + ePEnd;
  G4ThreeVector extentnegEnd               = referencePositionEnd   + eNEnd;
 
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "start position (global):       " << referencePositionStart << G4endl;
  G4cout << "end position (global):         " << referencePositionEnd   << G4endl;
  G4cout << "local extent +ve:              " << eP                     << G4endl;
  G4cout << "local extent -ve:              " << eN                     << G4endl;
  G4cout << "extent +ve at start in global: " << ePStart                << G4endl;
  G4cout << "extent -ve at start in global: " << eNStart                << G4endl;
  G4cout << "extent +ve at end in global:   " << ePEnd                  << G4endl;
  G4cout << "extent -ve at end in global:   " << eNEnd                  << G4endl;
  G4cout << "current global extent +ve:     " << maximumExtentPositive  << G4endl;
  G4cout << "current global extent -ve:     " << maximumExtentNegative  << G4endl;
#endif
  
  // loop over each size and compare to cumulative extent
  // do this at the start and end to be sure for long components
  // start
  for (int i=0; i<3; i++)
    {
      if (extentposStart[i] > maximumExtentPositive[i])
	{maximumExtentPositive[i] = extentposStart[i];}
      if (extentnegStart[i] < maximumExtentNegative[i])
	{maximumExtentNegative[i] = extentnegStart[i];}
    }
  // end
  for (int i=0; i<3; i++)
    {
      if (extentposEnd[i] > maximumExtentPositive[i])
	{maximumExtentPositive[i] = extentposEnd[i];}
      if (extentnegEnd[i] < maximumExtentNegative[i])
	{maximumExtentNegative[i] = extentnegEnd[i];}
    }
  // end comparing negative extents with positive world just in case
  for (int i=0; i<3; i++)
    {
      if (extentnegEnd[i] > maximumExtentPositive[i])
	{maximumExtentPositive[i] = extentnegEnd[i];}
    }
#ifdef BDSDEBUG
  G4cout << "new global extent +ve:         " << maximumExtentPositive << G4endl;
  G4cout << "new global extent -ve:         " << maximumExtentNegative << G4endl;
#endif
}
