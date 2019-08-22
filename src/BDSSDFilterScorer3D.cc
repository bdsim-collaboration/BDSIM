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
#include "BDSSDFilterScorer3D.hh"
#include "BDSScorerInfo.hh"
#include "BDSSDFilterTime.hh"
#include "BDSSDFilterLogicalVolume.hh"
#include "BDSSDFilterMaterial.hh"
#include "BDSMaterials.hh"

#include "globals.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSDFilter.hh"

#include "BDSSDFilterScorer3D.hh"

BDSSDFilterScorer3D::BDSSDFilterScorer3D(G4String             name,
					 const BDSScorerInfo* info):
  G4VSDFilter(name),
  particleWithKineticEnergyFilter(nullptr),
  timeFilter(nullptr),
  volumeFilter(nullptr),
  materialFilter(nullptr)
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
      timeFilter = new BDSSDFilterTime("time_filter",
				       info->minimumTime,
				       info->maximumTime);
    }

  // For the ambient dose, make a filter to only have the world volume
  /*
  if ((info->scorerType.underlying()) == BDSScorerType::ambientdose)
    {
      volumeFilter = new BDSSDFilterLogicalVolume("volume_filter",
						  BDSAcceleratorModel::Instance()->WorldLV());
    }
  */

  if (!(info->material.empty())) {
      // If we have multiple material, put everything in a vector.
      std::vector<G4Material*> materialVector;
      std::istringstream ss(info->material);
      G4String token;

      while (ss >> token)
      {
          materialVector.push_back(BDSMaterials::Instance()->GetMaterial(token));
      }
      /*
      while(std::getline(ss, token, ' '))
      {
          materialVector.push_back(BDSMaterials::Instance()->GetMaterial(token));
      }
       */
      materialFilter = new BDSSDFilterMaterial("material_filter",
                                               materialVector);

  }
}

BDSSDFilterScorer3D::~BDSSDFilterScorer3D()
{;}

G4bool BDSSDFilterScorer3D::Accept(const G4Step* aStep) const
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
  if (materialFilter)
    {
        if (!materialFilter->Accept(aStep))
        {return false;}
    }
  return true;
}
