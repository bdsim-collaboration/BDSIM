/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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

#ifndef BDSRadiationQuantity3D_H
#define BDSRadiationQuantity3D_H

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include "G4Physics2DVector.hh"
#include "G4VDataSetAlgorithm.hh"
#include <vector>


using namespace std;



class BDSScorerQuantity3D : public G4VPrimitiveScorer
{
public: // with description

    BDSScorerQuantity3D(const G4String scorer_name ="scorer", const G4String filename="",
    G4int ni=1,G4int nj=1, G4int nk=1,
    G4int depi=2, G4int depj=1, G4int depk=0);

    virtual ~BDSScorerQuantity3D() override;


protected: // with description
    G4bool ProcessHits(G4Step*,G4TouchableHistory*) override;
    G4int GetIndex(G4Step* aStep) override;

public:
    void Initialize(G4HCofThisEvent*) override;
    void EndOfEvent(G4HCofThisEvent*) override;
    void clear() override;

private:
  G4int HCID3D;
  G4THitsMap<G4double>* EvtMap3D;
  G4int fDepthi, fDepthj, fDepthk;
  G4PhysicsVector* conversionFactor;
};


#endif
