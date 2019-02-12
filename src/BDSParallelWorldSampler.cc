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
#include "BDSApertureInfo.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParser.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSamplerSD.hh"
#include "BDSSamplerType.hh"
#include "BDSUtilities.hh"

#include "parser/samplerplacement.h"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>


BDSParallelWorldSampler::BDSParallelWorldSampler(G4String name):
  G4VUserParallelWorld("SamplerWorld_" + name),
  suffix(name),
  samplerWorldVis(nullptr),
  generalPlane(nullptr)
{;}

BDSParallelWorldSampler::~BDSParallelWorldSampler()
{
  for (auto placement : placements)
    {delete placement;}
  for (auto sampler : samplers)
    {delete sampler;}
  delete samplerWorldVis;
  delete generalPlane;
}

void BDSParallelWorldSampler::Construct()
{
  G4VPhysicalVolume* samplerWorld   = GetWorld();
  G4LogicalVolume*   samplerWorldLV = samplerWorld->GetLogicalVolume();

  samplerWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->VisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  samplerWorldLV->SetVisAttributes(samplerWorldVis);
  
  const G4double samplerRadius = 0.5*BDSGlobalConstants::Instance()->SamplerDiameter();
  const BDSBeamline* beamline  = BDSAcceleratorModel::Instance()->BeamlineMain();
  const G4bool checkOverlaps   = BDSGlobalConstants::Instance()->CheckOverlaps();

  // Construct the one sampler typically used for a general sampler
  generalPlane = new BDSSamplerPlane("Plane_sampler", samplerRadius);
  samplers.push_back(generalPlane); // register it

  // For each element in the beamline construct and place the appropriate type
  // of sampler if required. Info encoded in BDSBeamlineElement instance
  for (auto element : *beamline)
    {
      BDSSamplerType samplerType = element->GetSamplerType();
      if (samplerType == BDSSamplerType::none)
	{continue;}
      // else must be a valid sampler
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Sampler type: " << element->GetSamplerType() << G4endl;
#endif
      G4String name = element->GetSamplerName();
      G4double sEnd = element->GetSPositionEnd();
      
      BDSSampler* sampler = nullptr;
      switch (samplerType.underlying())
	{
	case BDSSamplerType::plane:
	  {sampler = generalPlane; break;}
	case BDSSamplerType::cylinder:
	  {
	    G4double length = element->GetAcceleratorComponent()->GetChordLength();
	    sampler = new BDSSamplerCylinder(name,
					     length,
					     samplerRadius);
	    samplers.push_back(sampler); // register it - memory management
	    break;
	  }
	default:
	  {break;} // leave as nullptr - shouldn't occur due to if at top
	}
      
      if (sampler)
	{
	  G4Transform3D* pt = new G4Transform3D(*element->GetSamplerPlacementTransform());

#ifdef BDSDEBUG
	  G4cout << "Translation: " << pt->getTranslation() << G4endl;
	  G4cout << "Rotation:    " << pt->getRotation()    << G4endl;
#endif
	  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(name,
									    sampler,
									    *pt,
									    sEnd,
									    element);
	  
	  // record placements for cleaning up at destruction.
	  G4PVPlacement* pl = new G4PVPlacement(*pt,              // placement transform
						sampler->GetContainerLogicalVolume(), // logical volume
						name + "_pv",     // name of placement
						samplerWorldLV,   // mother volume
						false,            // no boolean operation
						samplerID,        // copy number
						checkOverlaps);
	  
	  placements.push_back(pl);
	}
    }

  // Now user customised samplers
  std::vector<GMAD::SamplerPlacement> samplerPlacements = BDSParser::Instance()->GetSamplerPlacements();

  for (const auto& samplerPlacement : samplerPlacements)
    {
      // use main beamline - in future, multiple beam lines
      G4Transform3D transform = BDSDetectorConstruction::CreatePlacementTransform(samplerPlacement, beamline);

      G4String samplerName = G4String(samplerPlacement.name);
      BDSApertureInfo* shape = nullptr;
      if (samplerPlacement.apertureModel.empty())
	{
	  shape = new BDSApertureInfo(samplerPlacement.shape,
				      samplerPlacement.aper1*CLHEP::m,
				      samplerPlacement.aper2*CLHEP::m,
				      samplerPlacement.aper3*CLHEP::m,
				      samplerPlacement.aper4*CLHEP::m);
	  // register for deletion
	  BDSAcceleratorModel::Instance()->RegisterAperture(samplerName + "_aperture", shape);
	}
      else
	{
	  shape = BDSAcceleratorModel::Instance()->Aperture(samplerPlacement.apertureModel);
	}
      BDSSampler* sampler = BuildSampler(samplerName,
					 samplerPlacement.shape,
					 samplerPlacement.aper1*CLHEP::m,
					 samplerPlacement.aper2*CLHEP::m,
					 samplerPlacement.aper3*CLHEP::m,
					 samplerPlacement.aper4*CLHEP::m);
      G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(samplerName,
									sampler,
									transform);

      G4PVPlacement* pl = new G4PVPlacement(transform,
					    sampler->GetContainerLogicalVolume(),
					    samplerName + "_pv",
					    samplerWorldLV,
					    false,
					    samplerID,
					    checkOverlaps);
      placements.push_back(pl);
    } 
}

BDSSampler* BDSParallelWorldSampler::BuildSampler(G4String        name,
						  G4String /*samplerShape*/,
						  G4double        aper1,
						  G4double        aper2,
						  G4double        aper3,
						  G4double        aper4) const
{
  if (!BDS::IsFinite(aper1))
    {
      G4cerr << "At least aper1 for samplerplacement \"" << name << "\" must be specified." << G4endl;
      exit(1);
    }
  BDSBeamPipeInfo bpi = BDSBeamPipeInfo("circularvacuum",
					aper1, aper2, aper3, aper4,
				        "vacuum", 1, "vacuum");

  BDSBeamPipe* bp = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name, 10*CLHEP::um, &bpi);

  BDSSampler* sampler = new BDSSampler(*bp);
  delete bp;
  return sampler;
}
