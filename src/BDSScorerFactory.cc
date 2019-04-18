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
#include "BDSException.hh"
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include "G4SDParticleWithEnergyFilter.hh"
#include "BDSScorerTimeFilter.hh"

#include "G4ScoringManager.hh"
#include "G4ScoringBox.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PSEnergyDeposit3D.hh"
#include "G4PSPopulation3D.hh"
#include "BDSScorerQuantity3D.hh"

BDSScorerFactory::BDSScorerFactory()
{;}

G4VPrimitiveScorer* BDSScorerFactory::CreateScorer(const BDSScorerInfo*      info,
						   const BDSHistBinMapper3D* mapper)
{
  // here we create the scorer with the informations inside BDSScorerInfo.
  G4VPrimitiveScorer* primitiveScorer = GetAppropriateScorer(info->name,
							     info->scorerType,
							     info->filename,
							     mapper);

  if (!info->particle) // no specific particle, i.e. all
    {return primitiveScorer;}
  
  G4String particleName = info->particle->GetParticleName();
  G4SDParticleWithEnergyFilter* scorer_filter= new G4SDParticleWithEnergyFilter("particle_filter",
										info->minimumEnergy,
										info->maximumEnergy);
  
  scorer_filter->add(particleName);
  primitiveScorer->SetFilter(scorer_filter);
  
  if(info->maximumTime != info->minimumTime)
    {
      BDSScorerTimeFilter *time_filter = new BDSScorerTimeFilter("time_filter", info->minimumTime, info->maximumTime);
      primitiveScorer->SetFilter(time_filter);
    }
  
  return primitiveScorer;
}

G4VPrimitiveScorer* BDSScorerFactory::GetAppropriateScorer(G4String name,
							   const BDSScorerType scorerType,
							   G4String filename,
							   const BDSHistBinMapper3D* mapper)
{
  G4VPrimitiveScorer* result = nullptr;
  switch (scorerType.underlying())
    {
    case BDSScorerType::deposited_dose:
      {result = new G4PSDoseDeposit3D(name); break;}
    case BDSScorerType::deposited_energy:
      {result = new G4PSEnergyDeposit3D(name); break;}
    case BDSScorerType::population:
      {
	G4PSPopulation3D* scorer = new G4PSPopulation3D(name);
	scorer->Weighted(TRUE);
	result = scorer;
	break;
      }
    case BDSScorerType::ambient_dose:
      {result = new BDSScorerQuantity3D(name,mapper,filename); break;}
     case BDSScorerType::activation:
      {result = new BDSScorerQuantity3D(name,mapper,filename); break;}
    default:
      {
	throw BDSException(__METHOD_NAME__, "unknown scorer type \"" + scorerType.ToString() + "\"");
	break;
      }
    }
  return result;
}
