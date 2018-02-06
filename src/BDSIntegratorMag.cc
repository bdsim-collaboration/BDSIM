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

