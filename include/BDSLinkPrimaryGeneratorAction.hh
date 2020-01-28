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
#ifndef BDSLINKPRIMARYGENERATORACTION_H
#define BDSLINKPRIMARYGENERATORACTION_H

#include "BDSExtent.hh"

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class BDSBunch;
class BDSOutputLoader;
class BDSPTCOneTurnMap;
class G4Event;
class G4ParticleGun;
/**
 * @brief Generates primary particle vertices using BDSBunch.
 *
 */

class BDSLinkPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
  /// Bunch must have a valid particle definition (ie not nullptr).
  explicit BDSLinkPrimaryGeneratorAction(BDSBunch* bunchIn);
  virtual ~BDSLinkPrimaryGeneratorAction();

  /// Main interface for Geant4. Prepare primary(ies) for the event.
  virtual void GeneratePrimaries(G4Event*);

  /// Set the world extent that particle coordinates will be checked against.
  inline void SetWorldExtent(const BDSExtent worldExtentIn) {worldExtent = worldExtentIn;}
  
private:  
  
  G4ParticleGun* particleGun;     ///< Geant4 particle gun that creates single particles.
  BDSBunch* bunch;                ///< BDSIM particle generator.  
  
  /// World extent that particle coordinates are checked against to ensure they're inside it.
  BDSExtent worldExtent;

  /// Can only get a G4ParticleDefinition for an ion when primary generator is called
  /// so cache the first time. This is the flag of that cache.
  //G4bool ionCached;
};

#endif
