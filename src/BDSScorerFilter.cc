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
#include "BDSAcceleratorModel.hh"
#include "BDSScorerFilter.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerTimeFilter.hh"
#include "BDSScorerVolumeFilter.hh"

#include "globals.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSDFilter.hh"

#include "BDSScorerFilter.hh"

BDSScorerFilter::BDSScorerFilter(G4String             name,
				 const BDSScorerInfo* info):
  G4VSDFilter(name),
  particleWithKineticEnergyFilter(nullptr),
  timeFilter(nullptr),
  volumeFilter(nullptr)
{
  // Define the different scorers depending info.

  // Particle with kinetic energy filter
  if (info->particle)
    {
      G4String particleName = info->particle->GetParticleName();
      particleWithKineticEnergyFilter = new G4SDParticleWithEnergyFilter("particle_filter",
                                                                         info->minimumEnergy,
                                                                         info->maximumEnergy);
      particleWithKineticEnergyFilter->add(particleName);
    }

  // Time filter
  if (info->maximumTime != info->minimumTime)
    {
      timeFilter = new BDSScorerTimeFilter("time_filter",
                                           info->minimumTime,
                                           info->maximumTime);
    }

  // For the ambient dose, make a filter to only have the world volume
  if((info->scorerType.underlying()) == BDSScorerType::ambientdose)
    {fVolumeFilter = new BDSScorerVolumeFilter("volume_filter");}
}

BDSScorerFilter::~BDSScorerFilter()
{;}

G4bool BDSScorerFilter::Accept(const G4Step* aStep) const
{
  if (particleWithKineticEnergyFilter)
    {
      if (!particleWithKineticEnergyFilter->Accept(aStep))
	{return false;}
    }
  if (timeFilter)
    {
      if (!timeFilter->Accept(aStep))
	{return false;}
    }
  if (volumeFilter)
    {
      if (!volumeFilter->Accept(aStep))
	{return false;}
    }
  return true;
}
