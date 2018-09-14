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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

BDSIntegratorTeleporter::BDSIntegratorTeleporter(G4Mag_EqRhs*  eqOfMIn,
						 G4Transform3D transformIn,
						 G4double      teleporterLengthIn,
						 BDSPTCOneTurnMap* oneTurnMapIn):
  BDSIntegratorMag(eqOfMIn,6),
  transform(transformIn),
  dPos(transform.getTranslation()),
  teleporterLength(teleporterLengthIn),
  oneTurnMap(oneTurnMapIn)
{
  newMethod = BDSGlobalConstants::Instance()->TeleporterFullTransform();
  useOneTurnMap = BDSPTCOneTurnMap::Instance()->IsInitialised();
}

void BDSIntegratorTeleporter::Stepper(const G4double yIn[],
				      const G4double /*dxdy*/[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  for(G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
  SetDistChord(0);

  G4int turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "turnstaken: " << turnstaken << G4endl;
#endif

  G4double lengthFraction = h / teleporterLength;
  
  // has to have completed at least 1 turn and be going forwards
  // must test for this to avoid backwards going particles getting stuck
  // also don't apply if for whatever reason the step length is less than half
  // the teleporter length -> this ensures only applied once
  if (turnstaken > 0 && yIn[5] > 0 && lengthFraction > 0.51)
    {
      G4ThreeVector globalPos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
      G4ThreeVector globalMom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);

      G4ThreeVector globalPosAfter;
      G4ThreeVector globalMomAfter;

      if (useOneTurnMap && BDSPTCOneTurnMap::Instance()->ShouldTeleporterApplyMap()) {
	std::cout << "I am applying the thing :) " << std::endl;
        const auto otm = BDSPTCOneTurnMap::Instance();
	const auto referenceMomentum = otm->GetReferenceMomentum();
        G4double x, px, y, py, deltaP;
	// pass by reference, returning the PTC coordinates:
        otm->GetThisTurn(x, px, y, py, deltaP);
	// Calculate the output momenta
	// ptc momenta are scaled by 1/p0, so we invert this to get
	// true momenta here.
	px *= referenceMomentum;
	py *= referenceMomentum;
	auto momentum = referenceMomentum * (deltaP + 1);
	// Calculate the momenta in pz.
	auto pz = std::sqrt(std::pow(momentum, 2) - std::pow(px, 2) - std::pow(py, 2));

	G4ThreeVector outLocalMomentum = G4ThreeVector(px, py, pz);
	// Calculate the output global positions
	BDSStep localPosMom = ConvertToLocal(globalPos, globalMom, h, false, thinElementLength);
	auto localPosition = localPosMom.PreStepPoint();
	auto outLocalPosition = G4ThreeVector(x, y, localPosition.z() + h);

        BDSStep globalPosDir =
	  ConvertToGlobalStep(outLocalPosition, outLocalMomentum, false);

	// Set the output positions and momenta
	globalPosAfter = globalPosDir.PreStepPoint();
	globalMomAfter = globalPosDir.PostStepPoint();
	std::cout << "applied the map." << std::endl;
      }

      else if (newMethod) { // new method - full tranfsorm3D - works in 3d
	                    // with beam line offset / rotation
	std::cout << "newMethod...!" << std::endl;
        globalPosAfter = globalPos + dPos;
        globalMomAfter = globalMom.transform(transform.getRotation());
	}
      else
	{// old method - only works with beam line pointing in global (0,0,1)
	  // global to local
	  BDSStep   localPosMom = ConvertToLocal(globalPos, globalMom, h, false, thinElementLength);
	  G4ThreeVector localPosition = localPosMom.PreStepPoint();
	  G4ThreeVector localMomentum = localPosMom.PostStepPoint();
	  G4ThreeVector localPositionAfter;
	  G4ThreeVector localMomentumAfter;
	  G4ThreeVector posDelta = transform.getTranslation();
	  localPositionAfter[0] = localPosition.x() - posDelta.x();
	  localPositionAfter[1] = localPosition.y() - posDelta.y();
	  localPositionAfter[2] = localPosition.z() + h;

	  BDSStep globalPosDir = ConvertToGlobalStep(localPositionAfter, localMomentum, false);
	  globalPosAfter = globalPosDir.PreStepPoint();
	  globalMomAfter = globalPosDir.PostStepPoint();
	}
      
      for (G4int i = 0; i < 3; i++)
	{
	  yOut[i]   = globalPosAfter[i];
	  yOut[i+3] = globalMomAfter[i];
	}
    }
  else
    {
      // move particle to other side without affecting anything
      yOut[0] = yIn[0];
      yOut[1] = yIn[1];
      if (yIn[5] > 0) // going forwards
	{yOut[2] = yIn[2] + h;}
      else // going backwards
	{yOut[2] = yIn[2] - h;}
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }
  
#ifdef BDSDEBUG
  G4ThreeVector inA  = G4ThreeVector(yIn[0],yIn[1],yIn[2]);
  G4ThreeVector inB  = G4ThreeVector(yIn[3],yIn[4],yIn[5]);
  G4ThreeVector outA = G4ThreeVector(yOut[0],yOut[1],yOut[2]);
  G4ThreeVector outB = G4ThreeVector(yOut[3],yOut[4],yOut[5]);
  std::ios_base::fmtflags ff = G4cout.flags(); // save cout flags
  G4cout.precision(10);
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "h (step length) " << h   /CLHEP::m << G4endl;
  G4cout << "Input x,y,z     " << inA /CLHEP::m << G4endl;
  G4cout << "Input px,py,pz  " << inB /CLHEP::m << G4endl;
  G4cout << "Output x,y,z    " << outA/CLHEP::m << G4endl;
  G4cout << "Output px,py,pz " << outB/CLHEP::m << G4endl;
  G4cout.flags(ff); // reset cout flags
#endif
}
