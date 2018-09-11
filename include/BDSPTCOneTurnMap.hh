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
#ifndef BDSPTCMAP_H
#define BDSPTCMAP_H

#include "globals.hh" // Geant4 typedefs

#include <vector>

namespace {
struct PTCMapTerm {
  G4double coefficient;
  G4int nx, npx, ny, npy, ndeltaP;
};

} // namespace

class BDSPTCOneTurnMap {
public:
  //! Default constructor
  BDSPTCOneTurnMap() = default;

  //! Copy constructor
  BDSPTCOneTurnMap(const BDSPTCOneTurnMap &other) = default;

  //! Move constructor
  BDSPTCOneTurnMap(BDSPTCOneTurnMap &&other) noexcept = default;

  //! Destructor
  virtual ~BDSPTCOneTurnMap() noexcept = default;

  //! Copy assignment operator
  BDSPTCOneTurnMap &operator=(const BDSPTCOneTurnMap &other) = default;

  //! Move assignment operator
  BDSPTCOneTurnMap &operator=(BDSPTCOneTurnMap &&other) noexcept = default;

  BDSPTCOneTurnMap(G4String path); // path to maptable file

  // public-facing should_apply?
  // This should be used by the terminator to decide if the teleporter
  // should use this instance (one turn map)
  G4bool isApplicable();

private:
  G4double evaluate(std::vector<PTCMapTerm> terms, G4double x, G4double px,
                    G4double y, G4double py, G4double deltaP);

  G4bool shouldApply; // Should we apply this map ?  This is intended
                     // to be set by the terminator (primary?  S0 != 0
                     // and first turn?) and read by the teleporter.

  // Evaluate this one turn map for the given coordinates.
  G4double evaluateX(G4double x, G4double px, G4double y, G4double py,
                     G4double deltaP);
  G4double evaluatePX(G4double x, G4double px, G4double y, G4double py,
                      G4double deltaP);
  G4double evaluateY(G4double x, G4double px, G4double y, G4double py,
                     G4double deltaP);
  G4double evaluatePY(G4double x, G4double px, G4double y, G4double py,
                      G4double deltaP);
  G4double evaluateDeltaP(G4double x, G4double px, G4double y, G4double py,
                          G4double deltaP);

  std::vector<PTCMapTerm> xTerms;
  std::vector<PTCMapTerm> yTerms;
  std::vector<PTCMapTerm> pxTerms;
  std::vector<PTCMapTerm> pyTerms;
  std::vector<PTCMapTerm> deltaPTerms;
};

#endif /* BDSPTCMAP_H */
