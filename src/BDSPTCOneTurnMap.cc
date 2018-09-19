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
  offsetS0AndOnFirstTurn(true) // by default do not say apply on first
			       // turn as strictly this is safer.
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

void BDSPTCOneTurnMap::SetInitialPrimaryCoordinates(const BDSParticleCoordsFullGlobal& coords,
						    G4bool offsetS0AndOnFirstTurnIn)
{
  //   // convert to PTC (PX = px / p0) coordinates.
  // G4cout << "GLOBAL PX = " << coords.global.xp << G4endl;
  // G4cout << "GLOBAL PY = " << coords.global.yp << G4endl;
  // G4cout << "GLOBAL PZ = " << coords.global.zp << G4endl;
  // G4cout << "LOCAL PX = " << coords.local.xp << G4endl;
  // G4cout << "LOCAL PY = " << coords.local.yp << G4endl;
  // G4cout << "LOCAL PZ = " << coords.local.zp << G4endl;
  // G4cout << "ENERGY = " << coords.local.totalEnergy << G4endl;
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
  // G4cout << "Updating coordinates... " << G4endl; //
  // G4cout << "xLastTurn = " << xLastTurn << G4endl;
  // G4cout << "pxLastTurn = " << pxLastTurn << G4endl;
  // G4cout << "yLastTurn = " << yLastTurn << G4endl;
  // G4cout << "pyLastTurn = " << pyLastTurn << G4endl;
  // G4cout << "deltaP = " << deltaPLastTurn << G4endl;
  // G4cout << "\n" << G4endl;

  // If we're using curvilinear then S0 != 0 and we shouldn't apply
  // map on first turn.  record this setting here for the teleporter's
  // consideration.
  offsetS0AndOnFirstTurn = offsetS0AndOnFirstTurnIn;
}


void BDSPTCOneTurnMap::GetThisTurn(G4double &x,
				   G4double &px,
				   G4double &y,
                                   G4double &py,
				   G4double &deltaP)
{

  // In short: lastTurnNumber exists to prevent the map being
  // applied multiple times in one turn.
  // The terminator is placed before the teleporter, so on the "first
  // turn" in the teleporter (where this method is called),
  // TurnsTaken() will actually return 2.  So lastTurnNumber, will be
  // one less than returned by TurnsTaken(), until it is incremented
  // below.  If (lastTurnNumber ==
  // BDSGlobalConstants::Instance()->TurnsTaken()), then the map has
  // already been applied on this turn.  In which case, return the
  // cached values below.
  if (lastTurnNumber == BDSGlobalConstants::Instance()->TurnsTaken() - 1)
    {
      lastTurnNumber++; // Increment the turn number
      G4double xOut  = evaluate(xTerms,  xLastTurn, pxLastTurn, yLastTurn, pyLastTurn, deltaPLastTurn);
      G4double pxOut = evaluate(pxTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn, deltaPLastTurn);
      G4double yOut  = evaluate(yTerms,  xLastTurn, pxLastTurn, yLastTurn, pyLastTurn, deltaPLastTurn);
      G4double pyOut = evaluate(pyTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn, deltaPLastTurn);
      G4double deltaPOut = evaluate(deltaPTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn, deltaPLastTurn);
      // Set the output and update the cached coordinates for maybe next
      // turn around the ring.  NOTE:  RETURNING PTC (NOT BDSIM)
      // COORDINATES!!!  IT is left to the user to convert back to
      // whatever they want.
      x      = xLastTurn      = xOut;
      px     = pxLastTurn     = pxOut;
      y      = yLastTurn      = yOut;
      py     = pyLastTurn     = pyOut;
      deltaP = deltaPLastTurn = deltaPOut;
    }
  else
    {
      G4cout << "Returning the cached values..." << G4endl;
      x = xLastTurn;
      px = pxLastTurn;
      y = yLastTurn;
      py = pyLastTurn;
      deltaP = deltaPLastTurn;
    }
  // std::cout << "The current turn number is..." << BDSGlobalConstants::Instance()->TurnsTaken()<< std::endl;
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

G4bool BDSPTCOneTurnMap::ShouldApply(G4double momentumIn) const
{
  G4double tol = 1e-5;
  G4bool should = (std::abs(momentumIn - initialPrimaryMomentum) < tol) && !offsetS0AndOnFirstTurn;
  return should;
}

// void BDSPTCOneTurnMap::SetTeleporterMapApplicability(G4Track* track) {
//   G4bool isPrimary = track->GetParentID() == 0;
//   G4bool firstTurnWithSOffset =
//       BDSGlobalConstants::Instance()->TurnsTaken() == 1 && offsetS0;
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
