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

#include "globals.hh" // Geant4 typedefs

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

BDSPTCOneTurnMap* BDSPTCOneTurnMap::instance = nullptr;

BDSPTCOneTurnMap* BDSPTCOneTurnMap::Instance() {

  auto filename = BDSGlobalConstants::Instance()->PTCOneTurnMapFileName();

  if (instance == nullptr) {
    if (filename == "") {
      instance = new BDSPTCOneTurnMap();
    }
    else {
      instance = new BDSPTCOneTurnMap(filename);
    }
  }
  return instance;
}

BDSPTCOneTurnMap::BDSPTCOneTurnMap() {
  initialised = false;
  shouldApply = false;
  useCurvilinear = true;
  referenceMomentum = -1; // dummy value.
}

BDSPTCOneTurnMap::BDSPTCOneTurnMap(G4String maptableFile) {
  std::ifstream infile(maptableFile);
  if (!infile) {
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
  while (std::getline(infile, line)) {
    if (line.at(0) == '@' || line.at(0) == '*' || line.at(0) == '$') {
      continue;
    }
    std::istringstream stream(line);

    stream >> name >> coefficient >> nVector >> dimensionality >> totalOrder >>
        nx >> npx >> ny >> npy >> ndeltaP >> nt;

    PTCMapTerm term{coefficient, nx, npx, ny, npy, ndeltaP};

    switch (nVector) {
    case 1: {
      xTerms.push_back(term);
      break;
    }
    case 2: {
      pxTerms.push_back(term);
      break;
    }
    case 3: {
      yTerms.push_back(term);
      break;
    }
    case 4: {
      pyTerms.push_back(term);
      break;
    }
    case 5: {
      deltaPTerms.push_back(term);
      break;
    }
    default:
      G4String message = "Unrecognised PTC term index.  Your maptable file "
                         "is perhaps malformed.";
      G4cerr << __METHOD_NAME__ << message << G4endl;
      exit(1);
      break;
    }
  }
  initialised = true;
  shouldApply = false;
  useCurvilinear = true;
  SetReferenceMomentum();
  SetMass();
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Loaded Map:" << maptableFile << G4endl;
#endif
}

void BDSPTCOneTurnMap::SetReferenceMomentum() {
  referenceMomentum =
      BDSGlobalConstants::Instance()->BeamParticleDefinition()->Momentum();
}

void BDSPTCOneTurnMap::SetMass() {
  mass = BDSGlobalConstants::Instance()->BeamParticleDefinition()->Mass();
}

void BDSPTCOneTurnMap::SetPrimaryCoordinates(BDSParticleCoordsFullGlobal coords,
                                             G4bool useCurvilinearIn) {
  UpdateCoordinates(coords);
  // If we're using curvilinear then S0 != 0 and we shouldn't apply
  // map on first turn.  record this setting here for the teleporter's
  // consideration.
  useCurvilinear = useCurvilinearIn;
}

void BDSPTCOneTurnMap::SetThisTurnResult() {
  std::cout << "Before SetThisTurnResult: " << std::endl;
  std::cout << "xLastTurn = " << xLastTurn << std::endl;
  std::cout << "pxLastTurn = " << pxLastTurn << std::endl;
  std::cout << "yLastTurn = " << yLastTurn << std::endl;
  std::cout << "pyLastTurn = " << pyLastTurn << std::endl;
  std::cout << "deltaPLastTurn = " << deltaPLastTurn << std::endl;
  std::cout << "\n";
  G4double x = evaluate(xTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn,
			deltaPLastTurn);
  G4double px = evaluate(pxTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn,
			 deltaPLastTurn);
  G4double y = evaluate(yTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn,
			deltaPLastTurn);
  G4double py = evaluate(pyTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn,
			 deltaPLastTurn);
  G4double deltaP = evaluate(deltaPTerms, xLastTurn, pxLastTurn, yLastTurn, pyLastTurn,
			     deltaPLastTurn);
  xLastTurn = x;
  pxLastTurn = px;
  yLastTurn = y;
  pyLastTurn = py;
  deltaPLastTurn = deltaP;

  std::cout << "After SetThisTurnResult: " << std::endl;
  std::cout << "xLastTurn = " << xLastTurn << std::endl;
  std::cout << "pxLastTurn = " << pxLastTurn << std::endl;
  std::cout << "yLastTurn = " << yLastTurn << std::endl;
  std::cout << "pyLastTurn = " << pyLastTurn << std::endl;
  std::cout << "deltaPLastTurn = " << deltaPLastTurn << std::endl;
  std::cout << "\n";

}

void BDSPTCOneTurnMap::GetThisTurn(G4double &x, G4double &px, G4double &y,
				   G4double &py, G4double &deltaP) {
  x = xLastTurn;
  px = pxLastTurn;
  y = yLastTurn;
  py = pyLastTurn;
  deltaP = deltaPLastTurn;
}

G4double BDSPTCOneTurnMap::evaluate(std::vector<PTCMapTerm> terms, G4double x,
                                    G4double px, G4double y, G4double py,
                                    G4double deltaP) {
  G4double result = 0;
  for (auto term : terms) {
    result += (term.coefficient * std::pow(x, term.nx) *
               std::pow(px, term.npx) * std::pow(y, term.ny) *
               std::pow(py, term.npy) * std::pow(deltaP, term.ndeltaP));
  }
  return result;
}

void BDSPTCOneTurnMap::UpdateCoordinates(BDSParticleCoordsFullGlobal coords) {
  // convert to PTC (PX = px / p0) coordinates.
  std::cout << "GLOBAL PX = " << coords.global.xp << std::endl;
  std::cout << "GLOBAL PY = " << coords.global.yp << std::endl;
  std::cout << "GLOBAL PZ = " << coords.global.zp << std::endl;
  std::cout << "LOCAL PX = " << coords.local.xp << std::endl;
  std::cout << "LOCAL PY = " << coords.local.yp << std::endl;
  std::cout << "LOCAL PZ = " << coords.local.zp << std::endl;
  std::cout << "ENERGY = " << coords.local.totalEnergy << std::endl;
  G4double totalMomentum = std::sqrt(std::pow(coords.local.totalEnergy, 2) - std::pow(mass, 2));
  // momentumMag =
  xLastTurn = coords.local.x / CLHEP::m;
  pxLastTurn = coords.global.xp * totalMomentum / referenceMomentum;
  yLastTurn = coords.local.y / CLHEP::m;
  pyLastTurn = coords.global.yp * totalMomentum / referenceMomentum;
  deltaPLastTurn = (totalMomentum - referenceMomentum) / referenceMomentum;
  std::cout << "Updating coordinates... " << std::endl;
  std::cout << "xLastTurn = " << xLastTurn << std::endl;
  std::cout << "pxLastTurn = " << pxLastTurn << std::endl;
  std::cout << "yLastTurn = " << yLastTurn << std::endl;
  std::cout << "pyLastTurn = " << pyLastTurn << std::endl;
  std::cout << "deltaP = " << deltaPLastTurn << std::endl;
  std::cout << "\n" << std::endl;

}

void BDSPTCOneTurnMap::SetTeleporterMapApplicability(G4Track* track) {
  G4bool isPrimary = track->GetParentID() == 0;
  G4bool firstTurnWithSOffset =
      BDSGlobalConstants::Instance()->TurnsTaken() == 1 && useCurvilinear;
  shouldApply = isPrimary && !firstTurnWithSOffset;
  // If on first turn with S offset then we must initialise the
  // coordinates for use from here onwards.
  if (isPrimary && firstTurnWithSOffset) {
    std::cout << "now we are also doing something" << std::endl;
    auto momentum = track->GetMomentum();
    auto position = track->GetPosition();
    xLastTurn = position.x() / CLHEP::m;
    yLastTurn = position.y() / CLHEP::m;
    pxLastTurn = momentum.x() / referenceMomentum;
    pyLastTurn = momentum.y() / referenceMomentum;
    deltaPLastTurn = (momentum.mag() - referenceMomentum) / referenceMomentum;
  }
}
