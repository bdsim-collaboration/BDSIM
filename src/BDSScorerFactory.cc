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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerAmbientDose3D.hh"
#include "BDSScorerQuantity3D.hh"
#include "BDSSDFilterAnd.hh"
#include "BDSSDFilterMaterial.hh"
#include "BDSSDFilterTime.hh"
#include "BDSScorerType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PSCellCharge.hh"
#include "G4PSCellCharge3D.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSEnergyDeposit3D.hh"
#include "G4PSPopulation.hh"
#include "G4PSPopulation3D.hh"
#include "G4ScoringBox.hh"
#include "G4ScoringManager.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "G4VPrimitiveScorer.hh"

BDSScorerFactory::BDSScorerFactory()
{;}

G4VPrimitiveScorer* BDSScorerFactory::CreateScorer(const BDSScorerInfo*      info,
						   const BDSHistBinMapper3D* mapper,
						   G4double*                 unit)
{
  // here we create the scorer with the informations inside BDSScorerInfo.
  G4VPrimitiveScorer* primitiveScorer = GetAppropriateScorer(info->name,
							     info->scorerType,
							     info->filename,
							     info->pathname,
							     mapper,
							     unit);

  BDSSDFilterAnd* filter = CreateFilter(info->name + "_scorer_filter", info);
  if (filter)
    {primitiveScorer->SetFilter(filter);}

  return primitiveScorer;
}

G4VPrimitiveScorer* BDSScorerFactory::GetAppropriateScorer(G4String                  name,
							   const BDSScorerType       scorerType,
							   G4String                  filename,
							   G4String                  pathname,
							   const BDSHistBinMapper3D* mapper,
							   G4double*                 unit)
{
  G4VPrimitiveScorer* result = nullptr;
  switch (scorerType.underlying())
    {
    case BDSScorerType::cellcharge:
      {result = new G4PSCellCharge(name);    break;}
    case BDSScorerType::cellcharge3d:
      {result = new G4PSCellCharge3D(name);  break;}
    case BDSScorerType::depositeddose:
      {result = new G4PSDoseDeposit(name);   break;}
    case BDSScorerType::depositeddose3d:
      {result = new G4PSDoseDeposit3D(name); break;}
    case BDSScorerType::depositedenergy:
      {result = new G4PSEnergyDeposit(name, "GeV"); break;}
    case BDSScorerType::depositedenergy3d:
      {result = new G4PSEnergyDeposit3D(name, "GeV"); break;}
    case BDSScorerType::population:
      {
	G4PSPopulation* scorer = new G4PSPopulation(name);
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::population3d:
      {
	G4PSPopulation3D* scorer = new G4PSPopulation3D(name);
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::ambientdose:
      {result = new BDSScorerAmbientDose3D(name,mapper,pathname); break;}
     case BDSScorerType::activation:
      {result = new BDSScorerQuantity3D(name,mapper,filename);break;}
    default:
      {
	throw BDSException(__METHOD_NAME__, "unknown scorer type \"" + scorerType.ToString() + "\"");
	break;
      }
    }
  if (unit)
    {*unit = result->GetUnitValue();}
  return result;
}

BDSSDFilterAnd* BDSScorerFactory::CreateFilter(G4String name,
					       const BDSScorerInfo* info) const
{
  BDSSDFilterAnd* result = new BDSSDFilterAnd(name, /*ownsFilters=*/true);

  if (info->particle)
    {
      G4String particleName = info->particle->GetParticleName();
      auto particleWithKineticEnergyFilter = new G4SDParticleWithEnergyFilter("particle_filter",
									      info->minimumEnergy,
									      info->maximumEnergy);
      particleWithKineticEnergyFilter->add(particleName);
      result->RegisterFilter(particleWithKineticEnergyFilter);
    }

  if (BDS::IsFinite(info->maximumTime) || BDS::IsFinite(info->minimumTime))
    {
      auto timeFilter = new BDSSDFilterTime("time_filter",
					    info->minimumTime,
					    info->maximumTime);
      result->RegisterFilter(timeFilter);
    }

  if (!(info->material.empty()))
    {
      // If we have multiple materials, put everything in a vector.
      std::vector<G4Material*> materialVector;
      std::istringstream ss(info->material);

      G4String word;
      while (ss >> word)
	{materialVector.push_back(BDSMaterials::Instance()->GetMaterial(word));}
      
      auto materialFilter = new BDSSDFilterMaterial("material_filter",
						    materialVector);
      result->RegisterFilter(materialFilter);
    }

  // if we didn't register any filters, just delete it
  if (result->size() == 0)
    {delete result; result = nullptr;}
  return result;
}
