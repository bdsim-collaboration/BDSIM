/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2023.

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
#include "BDSBunchFileBased.hh"

#include "parser/beam.h"

BDSBunchFileBased::BDSBunchFileBased(const G4String& distributionName):
  BDSBunch(distributionName),
  nOriginalEvents(0),
  nEventsInFile(0),
  nEventsInFileSkipped(0),
  distrFileLoop(false),
  distrFileLoopNTimes(0)
{;}

BDSBunchFileBased::~BDSBunchFileBased() 
{;}

void BDSBunchFileBased::SetOptions(const BDSParticleDefinition* beamParticle,
                                   const GMAD::Beam& beam,
                                   const BDSBunchType& distrType,
                                   G4Transform3D beamlineTransformIn,
                                   const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  distrFileLoop = beam.distrFileLoop;
  distrFileLoopNTimes = beam.distrFileLoopNTimes;
}