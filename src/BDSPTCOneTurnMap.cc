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

#include "BDSPTCOneTurnMap.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // Geant4 typedefs

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>


BDSPTCOneTurnMap::BDSPTCOneTurnMap(G4String maptableFile):
  beamOffsetS0(false)
{
  G4String filePath = BDS::GetFullPath(maptableFile);
  G4cout << __METHOD_NAME__ << "Using map table " << filePath << G4endl;
  std::ifstream infile(filePath);
  if (!infile)
    {
      G4String message = "Failed to read maptable: " + maptableFile;
      G4cerr << __METHOD_NAME__ << message << " Exiting. " << G4endl;
      exit(1);
    }

  // The columns of the maptable TFS (read into below with the stringsteam).
  G4String name = "";
  G4double coefficient = 0;
  G4int nVector = 0;
  G4int dimensionality = 0;
  G4int totalOrder = 0;
  G4int nx = 0;
  G4int npx = 0;
  G4int ny = 0;
  G4int npy = 0;
  G4int ndeltaP = 0;
  G4int nt = 0;

  G4String line = "";
  while (std::getline(infile, line))
    {
      if (line.at(0) == '@' || line.at(0) == '*' || line.at(0) == '$')
	{continue;}
      std::istringstream stream(line);

      stream >> name >> coefficient >> nVector >> dimensionality >> totalOrder >>
        nx >> npx >> ny >> npy >> ndeltaP >> nt;

      PTCMapTerm term{coefficient, nx, npx, ny, npy, ndeltaP};

      switch (nVector)
	{
	case 1:
	  {xTerms.push_back(term); break;}
	case 2:
	  {pxTerms.push_back(term); break;}
	case 3:
	  {yTerms.push_back(term);  break;}
	case 4:
	  {pyTerms.push_back(term); break;}
	case 5:
	  {deltaPTerms.push_back(term); break;}
	default:
	  {
	    G4String message = "Unrecognised PTC term index.  Your maptable file "
	      "is perhaps malformed.";
	    G4cerr << __METHOD_NAME__ << message << G4endl;
	    exit(1);
	    break;
	  }
	}
      SetReferenceMomentum();
      SetMass();
#ifdef BDSDEBUG
      G4cout << __FUNCTION__ << "> Loaded Map:" << maptableFile << G4endl;
#endif
    }
}

void BDSPTCOneTurnMap::SetReferenceMomentum()
{
  referenceMomentum =  BDSGlobalConstants::Instance()->BeamParticleDefinition()->Momentum();
}

void BDSPTCOneTurnMap::SetMass()
{
  mass = BDSGlobalConstants::Instance()->BeamParticleDefinition()->Mass();
}

void BDSPTCOneTurnMap::SetInitialPrimaryCoordinates(
    const BDSParticleCoordsFullGlobal &coords, G4bool beamOffsetS0In) {
  lastTurnNumber = BDSGlobalConstants::Instance()->TurnsTaken();
  initialPrimaryMomentum =
      std::sqrt(std::pow(coords.local.totalEnergy, 2) - std::pow(mass, 2));
  // Converting to PTC Coordinates:
  xLastTurn = coords.local.x / CLHEP::m;
  pxLastTurn = coords.global.xp * initialPrimaryMomentum / referenceMomentum;
  yLastTurn = coords.local.y / CLHEP::m;
  pyLastTurn = coords.global.yp * initialPrimaryMomentum / referenceMomentum;
  deltaPLastTurn =
      (initialPrimaryMomentum - referenceMomentum) / referenceMomentum;

  // If we're using curvilinear then S0 != 0 and we shouldn't apply
  // map on first turn.  record this setting here for the teleporter's
  // consideration.
  G4cout << "beamOffsetS0 = " << beamOffsetS0 << G4endl;

  beamOffsetS0 = beamOffsetS0In;
}

void BDSPTCOneTurnMap::GetThisTurn(G4double &x,
				   G4double &px,
				   G4double &y,
                                   G4double &py,
				   G4double &pz,
				   G4int turnstaken)
{
  G4double xOut = 0.0;
  G4double yOut = 0.0;
  G4double pxOut = 0.0;
  G4double pyOut = 0.0;
  G4double pzOut = 0.0;
  G4double deltaPOut = 0.0;

  // In short: lastTurnNumber exists to prevent the map being
  // applied multiple times in one turn.
  // The terminator is placed before the teleporter, so on the "first
  // turn" in the teleporter (where this method is called),
  // TurnsTaken() will actually return 2.  So lastTurnNumber, will be
  // one less than returned by TurnsTaken(), until it is incremented
  // below.  If (lastTurnNumber ==
  // turnstaken (turn number seen in the TeleporterIntegrator), then
  // the map has already been applied on this turn.  In which case,
  // return the cached values below.
  if (lastTurnNumber < turnstaken)
    {
      G4cout << __METHOD_NAME__ << "Applying Map: " << G4endl;
      G4cout << "Before map application: " << G4endl;
      G4cout << "xLastTurn = " << xLastTurn << G4endl;
      G4cout << "pxLastTurn = " << pxLastTurn << G4endl;
      G4cout << "yLastTurn = " << yLastTurn << G4endl;
      G4cout << "pyLastTurn = " << pyLastTurn << G4endl;
      lastTurnNumber = turnstaken;

      xOut = evaluate(xTerms,
		      xLastTurn, pxLastTurn,
		      yLastTurn, pyLastTurn,
		      deltaPLastTurn);
      pxOut = evaluate(pxTerms,
		       xLastTurn, pxLastTurn,
		       yLastTurn, pyLastTurn,
		       deltaPLastTurn);
      yOut = evaluate(yTerms,
		      xLastTurn, pxLastTurn,
		      yLastTurn, pyLastTurn,
		      deltaPLastTurn);
      pyOut = evaluate(pyTerms,
		       xLastTurn, pxLastTurn,
		       yLastTurn, pyLastTurn,
		       deltaPLastTurn);
      deltaPOut = evaluate(deltaPTerms,
			   xLastTurn, pxLastTurn,
			   yLastTurn, pyLastTurn,
			   deltaPLastTurn);
      // Cache results for next turn.  Do it here, before we convert
      // to BDSIM coordinates.
      xLastTurn      = xOut;
      pxLastTurn     = pxOut;
      yLastTurn      = yOut;
      pyLastTurn     = pyOut;
      deltaPLastTurn = deltaPOut;

      G4cout << "After map application: " << G4endl;
      G4cout << "xOut = " << xOut << G4endl;
      G4cout << "pxOut = " << pxOut << G4endl;
      G4cout << "yOut = " << yOut << G4endl;
      G4cout << "pyOut = " << pyOut << G4endl;


    }
  else
    {
      G4cout << __METHOD_NAME__ << "Returning Cached Values:" << G4endl;
      xOut      = xLastTurn;
      pxOut     = pxLastTurn;
      yOut      = yLastTurn;
      pyOut     = pyLastTurn;
      deltaPOut = deltaPLastTurn;
      G4cout << "xOut = " << xOut << G4endl;
      G4cout << "pxOut = " << pxOut << G4endl;
      G4cout << "yOut = " << yOut << G4endl;
      G4cout << "pyOut = " << pyOut << G4endl;
    }

  // Now convert to BDSIM units:
  // Convert local positions from metres to millimetres.
  xOut *= CLHEP::m;
  yOut *= CLHEP::m;

  // PTC momenta are scaled w.r.t the _REFERENCE_MOMENTUM_.
  pxOut *= referenceMomentum;
  pyOut *= referenceMomentum;
  // by defn the particle has the initial primary momentum, which we
  // used to calculate pz.
  pzOut = std::sqrt(initialPrimaryMomentum - std::pow(px,2) - std::pow(py,2));

  // Now set output for arguments passed by reference.
  x  = xOut;
  px = pxOut;
  y  = yOut;
  py = pyOut;
  pz = pzOut;

}

G4double BDSPTCOneTurnMap::evaluate(std::vector<PTCMapTerm>& terms,
				    G4double x,
                                    G4double px,
				    G4double y,
				    G4double py,
                                    G4double deltaP)
{
  G4double result = 0;
  for (const auto& term : terms)
    {
      result += (term.coefficient
		 * std::pow(x, term.nx)
		 * std::pow(px, term.npx)
		 * std::pow(y, term.ny)
		 * std::pow(py, term.npy)
		 * std::pow(deltaP, term.ndeltaP));
    }
  return result;
}

G4bool BDSPTCOneTurnMap::ShouldApply(G4double momentumIn,
				     G4int turnstaken) const
{
  // We have to use the externally provided turnstaken rather than
  // internal lastTurnNumber so that the OTM is definitely not applied
  // in this case.  2 and not 1 because teleporter comes after
  // terminator, where the turn number is incremented.
  G4bool offsetBeamS0AndOnFirstTurn = beamOffsetS0 && turnstaken == 2;

  G4double tol = 1e-5;
  G4bool unchangedMomentum =
      (std::abs(momentumIn - initialPrimaryMomentum) < tol);
  auto should = unchangedMomentum && !offsetBeamS0AndOnFirstTurn;
  G4cout << "should apply beamOffsetS0In? "
            << beamOffsetS0 << G4endl;

  G4cout << "should? " << should << G4endl;
  return should;
}

void BDSPTCOneTurnMap::UpdateCoordinates(G4ThreeVector localPosition,
                                         G4ThreeVector localMomentum) {
  // This method is called in the integrator if the OTM is active but
  // NOT applicable.  So given that the TeleporterIntegrator will be called
  // multiple times, whatever happens here, it should not suddenly
  // make the OTM applicable for subsequent calls to the Teleporter
  // stepper on the same turn.

  xLastTurn = localPosition.x();
  yLastTurn = localPosition.y();
  pxLastTurn = localMomentum.x() / referenceMomentum;
  pyLastTurn = localMomentum.y() / referenceMomentum;
  // deltaPLastTurn assumed to not change between turns for 5D map.
  G4cout << __METHOD_NAME__ << "Updating map coords without use of map:" << G4endl;
  G4cout << "xLastTurn = " << xLastTurn << G4endl;
  G4cout << "yLastTurn = " << yLastTurn << G4endl;
  G4cout << "pxLastTurn = " << pxLastTurn  << G4endl;
  G4cout << "pyLastTurn = " << pyLastTurn << G4endl;
}

// void BDSPTCOneTurnMap::SetTeleporterMapApplicability(G4Track* track) {
//   G4bool isPrimary = track->GetParentID() == 0;
//   G4bool firstTurnWithSOffset =
//       BDSGlobalConstants::Instance()->TurnsTaken() == 1 && beamOffsetS0;
//   shouldApply = isPrimary && !firstTurnWithSOffset;
//   // If on first turn with S offset then we must initialise the
//   // coordinates for use from here onwards.
//   if (isPrimary && firstTurnWithSOffset) {
//     G4cout << "now we are also doing something" << G4endl;
//     auto momentum = track->GetMomentum();
//     auto position = track->GetPosition();
//     xLastTurn = position.x() / CLHEP::m;
//     yLastTurn = position.y() / CLHEP::m;
//     pxLastTurn = momentum.x() / referenceMomentum;
//     pyLastTurn = momentum.y() / referenceMomentum;
//     deltaPLastTurn = (momentum.mag() - referenceMomentum) / referenceMomentum;
//   }
// }
