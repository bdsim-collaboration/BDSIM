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
#ifndef BDSBUNCHEVENTGENERATOR_H
#define BDSBUNCHEVENTGENERATOR_H 

#include "BDSBunch.hh"

#include "G4Types.hh"

#include <vector>

/**
 * @brief A wrapper of BDSBunch to include a filter for the events
 * loaded by an event generator.
 * 
 * @author Laurie Nevay
 */

class BDSBunchEventGenerator: public BDSBunch
{
public: 
  BDSBunchEventGenerator(); 
  virtual ~BDSBunchEventGenerator(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchEventGenerator& operator=(const BDSBunchEventGenerator&) = delete;
  BDSBunchEventGenerator(BDSBunchEventGenerator&) = delete;
  /// @}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam&            beam,
			  const BDSBunchType&          distrType,
			  G4Transform3D                beamlineTransformIn = G4Transform3D::Identity,
			  G4double                     beamlineS = 0);
  virtual void CheckParameters();

  G4bool AcceptParticle(const BDSParticleCoordsFull& coords,
			G4double kineticEnergy,
			G4int    pdgID);
  
protected:
  void ParseAcceptedParticleIDs();

  
  G4double eventGeneratorMinX;
  G4double eventGeneratorMaxX;
  G4double eventGeneratorMinY;
  G4double eventGeneratorMaxY;
  G4double eventGeneratorMinZ;
  G4double eventGeneratorMaxZ;
  G4double eventGeneratorMinXp;
  G4double eventGeneratorMaxXp;
  G4double eventGeneratorMinYp;
  G4double eventGeneratorMaxYp;
  G4double eventGeneratorMinZp;
  G4double eventGeneratorMaxZp;
  G4double eventGeneratorMinT;
  G4double eventGeneratorMaxT;
  G4double eventGeneratorMinEK;
  G4double eventGeneratorMaxEK;
  std::vector<G4int> acceptedParticles;

private:
  G4bool firstTime;
  G4String acceptedParticlesString;
};

#endif
