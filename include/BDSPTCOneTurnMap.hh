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
#include <set>

namespace
{
  struct PTCMapTerm
  {
    G4double coefficient;
    G4int nx;
    G4int npx;
    G4int ny;
    G4int npy;
    G4int ndeltaP;
  };
}; // namespace

// Note: This class uses PTC units internally for calculating the
// result of the map.

class BDSPTCOneTurnMap
{
public:
  /// Default constructor
  BDSPTCOneTurnMap() = delete;

  /// Copy constructor
  BDSPTCOneTurnMap(const BDSPTCOneTurnMap &other) = default;

  /// Move constructor
  BDSPTCOneTurnMap(BDSPTCOneTurnMap &&other) noexcept = default;

  /// Destructor
  virtual ~BDSPTCOneTurnMap() noexcept = default;

  /// Copy assignment operator
  BDSPTCOneTurnMap &operator=(const BDSPTCOneTurnMap &other) = default;

  /// Move assignment operator
  BDSPTCOneTurnMap &operator=(BDSPTCOneTurnMap &&other) noexcept = default;

  BDSPTCOneTurnMap(G4String path); ///< Path to maptable file.

  G4bool ShouldApplyToPrimary(G4int turnstaken);
  // Decides whether or not this should be applied.  Can add more

  void SetBeamParameters(G4double referenceMomentum, G4double mass);
  void SetInitialPrimaryCoordinates(const BDSParticleCoordsFullGlobal& coords,
				    G4bool offsetS0);
  void UpdateCoordinates(G4ThreeVector localPosition,
			 G4ThreeVector localMomentum);
  G4bool IsBeamOffsetS0() { return beamOffsetS0; }

  void GetThisTurn(G4double &x,
		   G4double &px,
		   G4double &y,
		   G4double &py,
		   G4double &pz,
		   G4int turnstaken);

  void SetReferenceMomentum();
  void SetMass();
  void SetInitialPrimaryMomentum(G4double in) {initialPrimaryMomentum = in;}

  G4double GetReferenceMomentum() const {return referenceMomentum;}

private:

  G4double evaluate(std::vector<PTCMapTerm>& terms,
		    G4double x,
		    G4double px,
                    G4double y,
		    G4double py,
		    G4double deltaP);

  G4double initialPrimaryMomentum;
  G4bool beamOffsetS0;

  G4double referenceMomentum;
  G4double mass;

  std::set<G4int> turnsScattered;

  G4int lastTurnNumber;
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

#endif
