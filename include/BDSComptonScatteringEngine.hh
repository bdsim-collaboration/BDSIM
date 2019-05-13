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
#ifndef BDSCOMPTONSCATTERINGENGINE_H
#define BDSCOMPTONSCATTERINGENGINE_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4DynamicParticle.hh"

#include "CLHEP/Units/PhysicalConstants.h"

class BDSComptonScatteringEngine
{
public:
    BDSComptonScatteringEngine();
    ~BDSComptonScatteringEngine();
    G4double CrossSection(G4double photonEnergy, G4int partiIDIn);
    void PerformCompton(G4int partiIDIn, G4ThreeVector boost);
    inline void setIncomingGamma(G4LorentzVector incomingGammaIn) {incomingGamma=incomingGammaIn;}
    inline void setIncomingElectron(G4LorentzVector incomingElectronIn) {incomingElectron=incomingElectronIn;}
    inline G4LorentzVector GetScatteredGamma()     {return scatteredGamma;};
    inline G4LorentzVector GetScatteredElectron()  {return scatteredElectron;};

private:
    G4LorentzVector incomingGamma;
    G4LorentzVector scatteredGamma;
    G4LorentzVector incomingElectron;
    G4LorentzVector scatteredElectron;




};

#endif