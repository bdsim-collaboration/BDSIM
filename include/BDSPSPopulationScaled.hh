/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2020.

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
#ifndef BDSPSPOPULATIONSCALED_H
#define BDSPSPOPULATIONSCALED_H

#include "globals.hh"
#include "G4THitsMap.hh"
#include "G4VPrimitiveScorer.hh"

class G4PhysicsVector;

/**
 * @brief Primitive scorer for population in a volume with a conversion factor based on angle and kinetic energy.
 *
 * Recorded particles are multiplied by a conversion factor based on the particle id, kinetic energy and incident angle.
 * The tables for the conversion factor are provided in a directory and file structure.
 *
 * The angle used for the lookup is the angle between the track and the detector axis.
 *
 * @author Andrey Abramov
 */

class BDSPSPopulationScaled: public G4VPrimitiveScorer
{

public:
    /// Constructor where no conversion factor file is provided and all cell fluxes just
    /// use conversion factor 1.0.
    BDSPSPopulationScaled(const G4String& name);

    /// Constructor where conversion factor file is provided and loaded into a map of maps of physics vectors.
    /// Population is multiplied by the factor as a function of the particle id, kinetic energy and angle.
    BDSPSPopulationScaled(const G4String&           scorerName,
                          const G4String&           pathname);

    virtual ~BDSPSPopulationScaled() override;

public:
    void   Initialize(G4HCofThisEvent* HCE) override;
    void   EndOfEvent(G4HCofThisEvent* HCE) override;
    void   clear() override;

    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;

    virtual G4double GetConversionFactor(G4int    particleID,
                                         G4double kineticEnergy, G4double angle) const;

    std::vector<G4String> LoadDirectoryContents(const G4String& dirname);
    G4double NearestNeighbourAngle(std::vector<G4double> const& vec, G4double value) const;
    G4int NearestNeighbourIonPID(std::vector<G4int> const& vec, G4int value) const;
    G4int GetZFromParticleID(G4int particleID) const;

private:
    G4int HCID;
    G4THitsMap<G4double>* EvtMap;

    void SetUnit(const G4String& unit) const;

    std::map< G4double, std::map<G4int, G4PhysicsVector*> > conversionFactors;
    std::map< G4double, std::vector<G4int> > ionParticleIDs;

};

#endif