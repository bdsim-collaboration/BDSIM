/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorMag.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

G4double BDSIntegratorMag::thinElementLength = -1; // mm

BDSIntegratorMag::BDSIntegratorMag(G4Mag_EqRhs* eqOfMIn,
				   G4int        nVariablesIn):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  eqOfM(eqOfMIn),
  nVariables(nVariablesIn),
  distChordPrivate(0)
{
  backupStepper = new G4ClassicalRK4(eqOfMIn, nVariablesIn);

  if (thinElementLength < 0)
    {thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();}
}
  
BDSIntegratorMag::~BDSIntegratorMag()
{
  delete backupStepper;
}

void BDSIntegratorMag::ConvertToGlobal(const G4ThreeVector& localPos,
				       const G4ThreeVector& localMom,
				       G4double             yOut[],
                                       const G4double       momScaling)
{
  BDSStep globalPosDir = ConvertToGlobalStep(localPos, localMom, false);
  G4ThreeVector GlobalPosition = globalPosDir.PreStepPoint();
  G4ThreeVector GlobalTangent  = globalPosDir.PostStepPoint();	
  GlobalTangent*=momScaling; // multiply the unit direction by magnitude to get momentum

  yOut[0] = GlobalPosition.x();
  yOut[1] = GlobalPosition.y();
  yOut[2] = GlobalPosition.z();

  yOut[3] = GlobalTangent.x();
  yOut[4] = GlobalTangent.y();
  yOut[5] = GlobalTangent.z();
}

BDSStep BDSIntegratorMag::GlobalToCurvilinear(G4ThreeVector position,
                                              G4ThreeVector unitMomentum,
                                              G4double      h,
                                              G4bool        useCurvilinearWorld)
{
  return ConvertToLocal(position, unitMomentum, h, useCurvilinearWorld);
}

BDSStep BDSIntegratorMag::GlobalToCurvilinear(BDSMagnetStrength const* strength,
                                              G4ThreeVector position,
                                              G4ThreeVector unitMomentum,
                                              G4double      h,
                                              G4bool        useCurvilinearWorld)
{
  G4double angle             = (*strength)["angle"];
  G4double arcLength         = (*strength)["length"];
  G4double radiusOfCurvature = arcLength / angle;
  G4double chordLength       = 2 * radiusOfCurvature * sin(angle*0.5);
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
  G4double sign = (angle < 0)? -1:1;

  G4ThreeVector arcCentre    = G4ThreeVector(-1*radiusAtChord,0,0);
  G4ThreeVector partVectToCentre  = arcCentre - localPos;
  G4double partToCentreDist  = partVectToCentre.mag();

  // angle along reference path, from -angle/2 to +angle/2
  G4double theta = acos(partVectToCentre.dot(arcCentre) / (arcCentre.mag() * partVectToCentre.mag()));

  // theta should be negative for first 'half' of dipole
  if (localZ < 0)
    {theta *= -1;}

  G4double CLXOffset  = sign*partToCentreDist - radiusOfCurvature;
  G4double distAlongS = theta * sign * radiusOfCurvature;

  G4ThreeVector localMomCL = localMom.rotate(-theta, localUnitF);
  G4ThreeVector localPosCL = G4ThreeVector(CLXOffset, localPos.y(), distAlongS);

/* Lauries algorithm
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

BDSStep BDSIntegratorMag::CurvilinearToGlobal(G4ThreeVector localPosition,
                                              G4ThreeVector localMomentum,
                                              G4bool        useCurvilinearWorld)
{
  return ConvertToGlobalStep(localPosition, localMomentum, useCurvilinearWorld);
}

BDSStep BDSIntegratorMag::CurvilinearToGlobal(BDSMagnetStrength const* strength,
                                              G4ThreeVector CLPosition,
                                              G4ThreeVector CLMomentum,
                                              G4bool        useCurvilinearWorld)
{
  G4double angle             = (*strength)["angle"];
  G4double arcLength         = (*strength)["length"];
  G4double radiusOfCurvature = arcLength / angle;
  G4double chordLength       = 2 * radiusOfCurvature * sin(angle*0.5);
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
  if (localZ < 0)
    {theta *= (-1*sign);}

  G4ThreeVector localPosition = G4ThreeVector(localX, CLPosition.y(), localZ);
  G4ThreeVector localMomentum = CLMomentum.rotate(theta, localUnitF);;

  return ConvertToGlobalStep(localPosition, localMomentum, useCurvilinearWorld);
}