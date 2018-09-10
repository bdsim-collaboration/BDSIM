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

#include "globals.hh" // Geant4 typedefs

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

BDSPTCOneTurnMap::BDSPTCOneTurnMap(G4String maptableFile) {
  std::ifstream infile(maptableFile);

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
}

G4double BDSPTCOneTurnMap::evaluateX(G4double x, G4double px, G4double y,
                                     G4double py, G4double deltaP) {
  return evaluate(xTerms, x, px, y, py, deltaP);
}

G4double BDSPTCOneTurnMap::evaluatePX(G4double x, G4double px, G4double y,
                                      G4double py, G4double deltaP) {
  return evaluate(pxTerms, x, px, y, py, deltaP);
}

G4double BDSPTCOneTurnMap::evaluateY(G4double x, G4double px, G4double y,
                                     G4double py, G4double deltaP) {
  return evaluate(yTerms, x, px, y, py, deltaP);
}

G4double BDSPTCOneTurnMap::evaluatePY(G4double x, G4double px, G4double y,
                                      G4double py, G4double deltaP) {
  return evaluate(pyTerms, x, px, y, py, deltaP);
}

G4double BDSPTCOneTurnMap::evaluateDeltaP(G4double x, G4double px, G4double y,
                                          G4double py, G4double deltaP) {
  return evaluate(deltaPTerms, x, px, y, py, deltaP);
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
