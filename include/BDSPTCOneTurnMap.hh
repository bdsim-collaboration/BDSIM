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
#ifndef BDSPTCONETURNMAP_H
#define BDSPTCONETURNMAP_H

#include "BDSParticleCoordsFullGlobal.hh"

#include "globals.hh" // Geant4 typedefs
#include "G4Track.hh"

#include <vector>

namespace {
struct PTCMapTerm {
  G4double coefficient;
  G4int nx, npx, ny, npy, ndeltaP;
};

} // namespace


// How this class intends to fit into BDSIM:
// This is instantiated in BDSGlobalConstants if ptcOneTurnMapFile != "".
// Reference momentum and particle mass is set in bdsim.cc
// Primary distribution is set in BDSPrimaryGeneratorAction, as well
// as informing if S0 != 0 (usecurvilinear).
// The decision for whether the map should be applied, or not, occurs
// in BDSPTCOneTurnMap::SetTeleporterApplicability, which is called in
// BDSTerminatorSD.cc.
// Finally, GetTeleporterApplicability is called in the teleporter.

class BDSPTCOneTurnMap {
public:
  //! Default constructor
  BDSPTCOneTurnMap() = delete;

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

  G4bool ShouldApply(G4double momentum) const;
  void UpdateCoordsForNextTurn();
  // Decides whether or not this should be applied.  Can add more

  void SetBeamParameters(G4double referenceMomentum, G4double mass);
  void SetInitialPrimaryCoordinates(BDSParticleCoordsFullGlobal coords,
				    G4bool offsetS0);
  void UpdateCoordinates(BDSParticleCoordsFullGlobal coords);


  void SetThisTurnResult();
  void GetThisTurn(G4double &x, G4double &px, G4double &y, G4double &py,
		   G4double &deltaP);

  void SetReferenceMomentum();
  void SetMass();
  void SetInitialPrimaryMomentum(G4double in) {initialPrimaryMomentum = in;}
  G4double GetReferenceMomentum() const {return referenceMomentum;}

private:

  G4double evaluate(std::vector<PTCMapTerm> terms, G4double x, G4double px,
                    G4double y, G4double py, G4double deltaP);


  G4double initialPrimaryMomentum;
  G4bool offsetS0AndOnFirstTurn;

  G4double referenceMomentum;
  G4double mass;


  G4double xLastTurn;
  G4double pxLastTurn;
  G4double yLastTurn;
  G4double pyLastTurn;
  G4double deltaPLastTurn;

  std::vector<PTCMapTerm> xTerms;
  std::vector<PTCMapTerm> yTerms;
  std::vector<PTCMapTerm> pxTerms;
  std::vector<PTCMapTerm> pyTerms;
  std::vector<PTCMapTerm> deltaPTerms;
};

#endif /* BDSPTCONETURNMAP_H */
