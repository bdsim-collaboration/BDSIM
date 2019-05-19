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
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerQuantity3D.hh"
#include "BDSScorerFilter.hh"
#include "BDSScorerType.hh"

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
							     mapper,
							     unit);

  BDSScorerFilter* filter = new BDSScorerFilter("filter", info);
  primitiveScorer->SetFilter(filter);

  return primitiveScorer;
}

G4VPrimitiveScorer* BDSScorerFactory::GetAppropriateScorer(G4String                  name,
							   const BDSScorerType       scorerType,
							   G4String                  filename,
							   const BDSHistBinMapper3D* mapper,
							   G4double*                 unit)
{
  G4VPrimitiveScorer* result = nullptr;
  switch (scorerType.underlying())
    {
    case BDSScorerType::cellcharge:
      {result = new G4PSCellCharge(name); break;}
    case BDSScorerType::cellcharge3d:
      {result = new G4PSCellCharge3D(name); break;}
    case BDSScorerType::depositeddose:
      {
	result = new G4PSDoseDeposit(name);
	if (unit)
	  {*unit = CLHEP::gray;}
	break;
      }
    case BDSScorerType::depositeddose3d:
      {
	result = new G4PSDoseDeposit3D(name);
	if (unit)
	  {*unit = CLHEP::gray;}
	break;
      }
    case BDSScorerType::depositedenergy:
      {
	result = new G4PSEnergyDeposit(name);
	if (unit)
	  {*unit = CLHEP::GeV;}
	break;
      }
    case BDSScorerType::depositedenergy3d:
      {
	result = new G4PSEnergyDeposit3D(name);
	if (unit)
	  {*unit = CLHEP::GeV;}
	break;
      }
    case BDSScorerType::population:
      {
	G4PSPopulation* scorer = new G4PSPopulation(name);
	scorer->Weighted(TRUE);
	result = scorer;
	break;
      }
    case BDSScorerType::population3d:
      {
	G4PSPopulation3D* scorer = new G4PSPopulation3D(name);
	scorer->Weighted(TRUE);
	result = scorer;
	break;
      }
    case BDSScorerType::ambientdose:
      {
	result = new BDSScorerQuantity3D(name,mapper,filename);
	break;
	// TBC unit
      }
     case BDSScorerType::activation:
      {
	result = new BDSScorerQuantity3D(name,mapper,filename);
	break;
	// TBC unit
      }
    default:
      {
	throw BDSException(__METHOD_NAME__, "unknown scorer type \"" + scorerType.ToString() + "\"");
	break;
      }
    }
  return result;
}
