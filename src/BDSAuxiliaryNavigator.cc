/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "G4Navigator.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"

G4Navigator*       BDSAuxiliaryNavigator::auxNavigator             = new G4Navigator();
G4Navigator*       BDSAuxiliaryNavigator::auxNavigatorCL           = new G4Navigator();
G4Navigator*       BDSAuxiliaryNavigator::auxNavigatorCLB          = new G4Navigator();
G4int              BDSAuxiliaryNavigator::numberOfInstances        = 0;
G4VPhysicalVolume* BDSAuxiliaryNavigator::worldPV                  = nullptr;
G4VPhysicalVolume* BDSAuxiliaryNavigator::curvilinearWorldPV       = nullptr;
G4VPhysicalVolume* BDSAuxiliaryNavigator::curvilinearBridgeWorldPV = nullptr;

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator():
  globalToLocal(G4AffineTransform()),
  localToGlobal(G4AffineTransform()),
  globalToLocalCL(G4AffineTransform()),
  localToGlobalCL(G4AffineTransform()),
  volumeMargin(0.1*CLHEP::mm)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  numberOfInstances++;
}

BDSAuxiliaryNavigator::~BDSAuxiliaryNavigator()
{
  // Only delete static navigator objects when last instance is deleted
  if (numberOfInstances == 1)
    {
      delete auxNavigator;    auxNavigator   = nullptr;
      delete auxNavigatorCL;  auxNavigatorCL = nullptr;
      delete auxNavigatorCLB; auxNavigatorCLB = nullptr;
    }
  numberOfInstances--;
}

G4VPhysicalVolume* BDSAuxiliaryNavigator::LocateGlobalPointAndSetup(const G4ThreeVector& point,
								    const G4ThreeVector* direction,
								    const G4bool pRelativeSearch,
								    const G4bool ignoreDirection,
								    G4bool useCurvilinear) const
{
  G4Navigator* nav = Navigator(useCurvilinear);
  auto selectedVol = nav->LocateGlobalPointAndSetup(point, direction,
					pRelativeSearch, ignoreDirection);

#ifdef BDSDEBUGNAV
  G4cout << "Point lookup " << selectedVol->GetName() << G4endl;
#endif

  // check if we accidentally fell between the gaps and found the world volume
  if (useCurvilinear && selectedVol == curvilinearWorldPV)
    {// try the bridge world next
      selectedVol = auxNavigatorCLB->LocateGlobalPointAndSetup(point, direction,
							       pRelativeSearch, ignoreDirection);
      // if we find a non-world volume, then good. if we find the world volume even
      // of the bridge world, it must really lie outside the curvilinear volumes
      // eitherway, we return that volume.
      return selectedVol;
    }
  else if (selectedVol == worldPV)
    {// try searching a little further along the step
      if (!direction) // no direction, so can't search
        {return selectedVol;}
      G4ThreeVector globalDirUnit = direction->unit();
      G4ThreeVector newPosition = point + volumeMargin*globalDirUnit;
      selectedVol = nav->LocateGlobalPointAndSetup(newPosition, direction,
						   pRelativeSearch, ignoreDirection);
#ifdef BDSDEBUGNAV
      G4cout << __METHOD_NAME__ << "New selected volume is: " << selectedVol->GetName() << G4endl;
#endif
      return selectedVol;
    }
  else
    {return selectedVol;}
}

G4VPhysicalVolume* BDSAuxiliaryNavigator::LocateGlobalPointAndSetup(G4Step const* const step,
								    G4bool useCurvilinear) const
{ // const pointer to const G4Step
  // 'pos' = post but has same length as pre so code looks better
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4StepPoint* posStepPoint = step->GetPostStepPoint();

  // average the points - the mid point should always lie inside the volume given
  // the way G4 does tracking.
  G4ThreeVector prePosition   = preStepPoint->GetPosition();
  G4ThreeVector postPosition  = posStepPoint->GetPosition();
  G4ThreeVector position      = (postPosition + prePosition)/2.0;
  G4ThreeVector globalDirUnit = (postPosition - prePosition).unit();
  
  G4Navigator* nav = Navigator(useCurvilinear);  // select navigator
  G4VPhysicalVolume* selectedVol = nav->LocateGlobalPointAndSetup(position, &globalDirUnit);

#ifdef BDSDEBUGNAV
  G4cout << __METHOD_NAME__ << selectedVol->GetName() << G4endl;
#endif
  // check if we accidentally fell between the gaps and found the world volume
  if (useCurvilinear && selectedVol == curvilinearWorldPV)
    {// try the bridge world next
      selectedVol = auxNavigatorCLB->LocateGlobalPointAndSetup(position, &globalDirUnit);
      // if we find a non-world volume, then good. if we find the world volume even
      // of the bridge world, it must really lie outside the curvilinear volumes
      // eitherway, we return that volume.
      return selectedVol;
    }
  else if (selectedVol == worldPV)
    {// try searching a little further along the step from the pre-step point
      G4ThreeVector newPosition = position + volumeMargin*globalDirUnit;
      selectedVol = nav->LocateGlobalPointAndSetup(position, &globalDirUnit);
#ifdef BDSDEBUGNAV
      G4cout << __METHOD_NAME__ << "New selected volume is: " << selectedVol->GetName() << G4endl;
#endif
      return selectedVol;
    }
  else
    {return selectedVol;}
}

BDSStep BDSAuxiliaryNavigator::ConvertToLocal(G4Step const* const step,
					      G4bool useCurvilinear) const
{
  auto selectedVol = LocateGlobalPointAndSetup(step, useCurvilinear);

#ifdef BDSDEBUGNAV
  G4cout << __METHOD_NAME__ << selectedVol->GetName() << G4endl;
#endif

  useCurvilinear ? InitialiseTransform(false, true) : InitialiseTransform(true, false);

  G4ThreeVector pre = GlobalToLocal(useCurvilinear).TransformPoint(step->GetPreStepPoint()->GetPosition());
  G4ThreeVector pos = GlobalToLocal(useCurvilinear).TransformPoint(step->GetPostStepPoint()->GetPosition());
  return BDSStep(pre, pos, selectedVol);
}

BDSStep BDSAuxiliaryNavigator::ConvertToLocal(const G4ThreeVector& globalPosition,
					      const G4ThreeVector& globalDirection,
					      const G4double       stepLength,
					      const G4bool&        useCurvilinear,
					      const G4double       marginLength) const
{
  G4ThreeVector point = globalPosition;
  // protect against boundary problems - use step length to sample into volume
  // however, sometimes the step length is stupidly long and the mid point may
  // lie outside the volume. Geant does this to evaluate the maximum length it
  // could take before breaking the tracking accuracy / bending limits even
  // though it clearly may leave the volume. Invoke a bit of knowledge about the
  // scale of the problem and sample only 1mm along.
  G4ThreeVector globalDirUnit = globalDirection.unit();
  if (stepLength > 1*CLHEP::mm) // too long - may go outside typical geometry length
    {point += globalDirUnit * marginLength;}
  else if (stepLength > 0) // must be a shorter length, obey it
    {point += globalDirUnit * (stepLength * 0.5);}
  // else pass: point = globalPosition
  
  auto selectedVol = LocateGlobalPointAndSetup(point,
					       &globalDirection,
					       true,  // relative search
					       false, // don't ignore direction, ie use it
					       useCurvilinear);
#ifdef BDSDEBUGNAV
  G4cout << __METHOD_NAME__ << selectedVol->GetName() << G4endl;
#endif
  
  useCurvilinear ? InitialiseTransform(false, true) : InitialiseTransform(true, false);
  const G4AffineTransform& aff = GlobalToLocal(useCurvilinear);
  G4ThreeVector localPos = aff.TransformPoint(globalPosition);
  G4ThreeVector localDir = aff.TransformAxis(globalDirection);
  return BDSStep(localPos, localDir, selectedVol);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4double globalPosition[3],
						    const G4bool&  useCurvilinear) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  return ConvertToLocal(globalPositionV, useCurvilinear);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4ThreeVector& globalPosition,
						    const G4bool&        useCurvilinear) const
{
  InitialiseTransform(globalPosition);
  return GlobalToLocal(useCurvilinear).TransformPoint(globalPosition);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4ThreeVector& globalAxis,
							const G4bool&        useCurvilinear) const
{
  return GlobalToLocal(useCurvilinear).TransformAxis(globalAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4double globalPosition[3],
							const G4double globalAxis[3],
							const G4bool&  useCurvilinear) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  G4ThreeVector globalAxisV(globalAxis[0], globalAxis[1], globalAxis[2]);
  return ConvertAxisToLocal(globalPositionV, globalAxisV, useCurvilinear);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4ThreeVector& globalPosition,
							const G4ThreeVector& globalAxis,
							const G4bool&        useCurvilinear) const
{
  InitialiseTransform(globalPosition);
  return GlobalToLocal(useCurvilinear).TransformAxis(globalAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& localAxis,
							 const G4bool&        useCurvilinear) const
{return LocalToGlobal(useCurvilinear).TransformAxis(localAxis);}

std::pair<G4ThreeVector, G4ThreeVector> BDSAuxiliaryNavigator::ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis,
										   const G4bool& useCurvilinear) const
{
  const G4AffineTransform& lToG = LocalToGlobal(useCurvilinear);
  G4ThreeVector globalB = lToG.TransformAxis(localAxis.first);
  G4ThreeVector globalE = lToG.TransformAxis(localAxis.second);
  return std::make_pair(globalB, globalE);
}										 

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& localPosition,
						     const G4bool&        useCurvilinear) const
{return LocalToGlobal(useCurvilinear).TransformPoint(localPosition);}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& globalPosition,
							 const G4ThreeVector& localAxis,
							 const G4bool&        useCurvilinear) const
{
  InitialiseTransform(globalPosition);
  return LocalToGlobal(useCurvilinear).TransformAxis(localAxis);
}

BDSStep BDSAuxiliaryNavigator::ConvertToGlobalStep(const G4ThreeVector& localPosition,
					       const G4ThreeVector& localDirection,
					       const G4bool&        useCurvilinear) const
{
  const G4AffineTransform& aff = LocalToGlobal(useCurvilinear);
  G4ThreeVector globalPos = aff.TransformPoint(localPosition);
  G4ThreeVector globalDir = aff.TransformAxis(localDirection);
  return BDSStep(globalPos, globalDir);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& globalPosition,
						     const G4ThreeVector& localPosition,
						     const G4bool&        useCurvilinear) const
{
  InitialiseTransform(globalPosition);
  return LocalToGlobal(useCurvilinear).TransformPoint(localPosition);
}

BDSStep BDSAuxiliaryNavigator::GlobalToCurvilinear(G4ThreeVector position,
                                              G4ThreeVector unitMomentum,
                                              G4double      h,
                                              G4bool        useCurvilinearWorld)
{
  return ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);
}

BDSStep BDSAuxiliaryNavigator::GlobalToCurvilinear(BDSMagnetStrength const* strength,
                                              G4ThreeVector position,
                                              G4ThreeVector unitMomentum,
                                              G4double      h,
                                              G4bool        useCurvilinearWorld,
                                              G4double      FCof)
{
  G4double angle             = (*strength)["angle"];
  G4double arcLength         = (*strength)["length"];
  G4double radiusOfCurvature = arcLength / angle;
  G4double radiusAtChord     = radiusOfCurvature * cos(angle*0.5);
  G4ThreeVector unitField    = G4ThreeVector(0,(*strength)["field"],0).unit();

  BDSStep local = ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);

  // Test on finite angle here. If the angle is 0, there is no need for a further transform.
  if (!BDS::IsFinite(angle))
    {return local;}

  G4ThreeVector localPos   = local.PreStepPoint();
  G4ThreeVector localMom   = local.PostStepPoint();
  G4double      localZ     = localPos.z();
  G4ThreeVector localUnitF = ConvertAxisToLocal(unitField, useCurvilinearWorld);

  // only find angle between particle and radiusAtChord in x-z plane,
  // conversion to CL shouldn't affect y co-ordinate but finite y co-ord would affect angle
  G4ThreeVector localXZPos        = G4ThreeVector(localPos.x(), 0, localPos.z());
  G4ThreeVector arcCentre         = G4ThreeVector(-1*radiusAtChord,0,0);
  G4ThreeVector partVectToCentre  = arcCentre - localXZPos;
  G4double partToCentreDist       = partVectToCentre.mag();

  // angle along reference path, from -angle/2 to +angle/2
  G4double theta = acos(partVectToCentre.dot(arcCentre) / (arcCentre.mag() * partVectToCentre.mag()));

  // theta should be negative for first 'half' of dipole
  if (localZ < 0)
    {theta *= -1;}

  // vector from origin to CL arc centre depends on sign of dipole angle
  G4double sign = (angle < 0)? -1:1;

  G4double CLXOffset  = sign*partToCentreDist - radiusOfCurvature;
  G4double distAlongS = theta * sign * radiusOfCurvature;

  // normalise momentum rotation to particle charge
  G4double charge = 0;
  charge = (FCof < 0)? -1: 1;

  G4double rotationAngle = theta*charge;

  G4ThreeVector localMomCL = localMom.rotate(rotationAngle, localUnitF);
  G4ThreeVector localPosCL = G4ThreeVector(CLXOffset, localPos.y(), distAlongS);

/* Lauries algorithm - keep for now
  // This will range from -angle/2 to +angle/2
  G4double partialAngle = atan(localZ / radiusAtChord);

  G4ThreeVector localMomCL = localMom.rotate(partialAngle, localUnitF);

  G4ThreeVector unitX      = G4ThreeVector(1,0,0);
  G4ThreeVector localUnitX = ConvertAxisToLocal(unitX, useCurvilinearWorld);
  G4double      dx         = radiusOfCurvature * (1 - cos(partialAngle));
  G4ThreeVector dpos       = localUnitX * dx;
  G4ThreeVector localPosCL = localPos + dpos;
*/

  return BDSStep(localPosCL, localMomCL);
}

BDSStep BDSAuxiliaryNavigator::CurvilinearToGlobal(G4ThreeVector localPosition,
                                              G4ThreeVector localMomentum,
                                              G4bool        useCurvilinearWorld)
{
  return ConvertToGlobalStep(localPosition, localMomentum, useCurvilinearWorld);
}

BDSStep BDSAuxiliaryNavigator::CurvilinearToGlobal(BDSMagnetStrength const* strength,
                                              G4ThreeVector CLPosition,
                                              G4ThreeVector CLMomentum,
                                              G4bool        useCurvilinearWorld,
                                              G4double      FCof)
{
  G4double angle             = (*strength)["angle"];
  G4double arcLength         = (*strength)["length"];
  G4double radiusOfCurvature = arcLength / angle;
  G4double radiusAtChord     = radiusOfCurvature * cos(angle*0.5);
  G4ThreeVector unitField    = G4ThreeVector(0,(*strength)["field"],0).unit();

  // Test on finite angle here. If the angle is 0, return convert to global transform.
  if (!BDS::IsFinite(angle))
    {return ConvertToGlobalStep(CLPosition, CLMomentum, useCurvilinearWorld);}

  G4double sign = (angle < 0)? 1:-1;
  G4ThreeVector localUnitF = ConvertAxisToLocal(unitField, useCurvilinearWorld);
  G4ThreeVector arcCentre  = G4ThreeVector(sign*radiusAtChord,0,0);

  G4double theta = CLPosition.z() / radiusOfCurvature;

  G4double partToCentreDist  = CLPosition.x() + radiusOfCurvature;
  G4double localZ = partToCentreDist * sin(theta);
  G4double localX = (partToCentreDist * cos(theta)) - radiusAtChord;

  // normalise momentum rotation to particle charge
  G4double charge = 0;
  charge = (FCof < 0)? -1: 1;

  // rotation angle should be negative for second 'half' of dipole
  G4double rotationAngle = std::abs(theta)*charge;
  if (localZ > 0)
    {rotationAngle *= -1;}

  G4ThreeVector localPosition = G4ThreeVector(localX, CLPosition.y(), localZ);
  G4ThreeVector localMomentum = CLMomentum.rotate(rotationAngle, localUnitF);;

  return ConvertToGlobalStep(localPosition, localMomentum, useCurvilinearWorld);
}

G4Navigator* BDSAuxiliaryNavigator::Navigator(G4bool curvilinear) const
{
  // condition ? case true : case false
  return curvilinear ? auxNavigatorCL : auxNavigator;
}

const G4AffineTransform& BDSAuxiliaryNavigator::GlobalToLocal(G4bool curvilinear) const
{
  return curvilinear ? globalToLocalCL : globalToLocal;
}

const G4AffineTransform& BDSAuxiliaryNavigator::LocalToGlobal(G4bool curvilinear) const
{
  return curvilinear ? localToGlobalCL : localToGlobal;
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4bool& massWorld,
						const G4bool& curvilinearWorld) const
{
  if (massWorld)
    {
      globalToLocal   = auxNavigator->GetGlobalToLocalTransform();
      localToGlobal   = auxNavigator->GetLocalToGlobalTransform();
    }
  if (curvilinearWorld)
    {
      globalToLocalCL = auxNavigatorCL->GetGlobalToLocalTransform();
      localToGlobalCL = auxNavigatorCL->GetLocalToGlobalTransform();
    }
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4ThreeVector& globalPosition) const
{
  auxNavigator->LocateGlobalPointAndSetup(globalPosition);
  auxNavigatorCL->LocateGlobalPointAndSetup(globalPosition);
  globalToLocal = auxNavigator->GetGlobalToLocalTransform();
  localToGlobal = auxNavigator->GetLocalToGlobalTransform();
  globalToLocalCL = auxNavigatorCL->GetGlobalToLocalTransform();
  localToGlobalCL = auxNavigatorCL->GetLocalToGlobalTransform();
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4ThreeVector &globalPosition,
                                                const G4ThreeVector &globalMomentum,
						const G4double stepLength)
{
  G4ThreeVector endPoint = globalPosition + globalMomentum.unit()*stepLength;
  G4ThreeVector midPoint = (endPoint + globalPosition) / 2;
  InitialiseTransform(midPoint);
}
