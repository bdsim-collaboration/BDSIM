/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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

#ifndef BDSUSERSPECIALCUTSPARALLEL_H
#define BDSUSERSPECIALCUTSPARALLEL_H

#include "G4ParallelWorldProcess.hh"

class G4Step;
class G4String;
class G4Track;

class BDSUserSpecialCutsParallel: public G4ParallelWorldProcess
{
public:
  explicit BDSUserSpecialCutsParallel(const G4String& nameIn);
  virtual ~BDSUserSpecialCutsParallel() = default;
  
  BDSUserSpecialCutsParallel(BDSUserSpecialCutsParallel&) = delete;
  
  virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                                        G4double /*previousStepSize*/,
                                                        G4ForceCondition* condition);

  virtual G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                          const G4Step&  aStep);
};

#endif
