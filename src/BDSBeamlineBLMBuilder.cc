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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlineBLMBuilder.hh"
#include "BDSBLM.hh"
#include "BDSBLMFactory.hh"
#include "BDSBLMRegistry.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSParser.hh"
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSSDManager.hh"
#include "BDSSimpleComponent.hh"
#include "BDSUtilities.hh"

#include "parser/blmplacement.h"
#include "parser/scorer.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "globals.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4VSensitiveDetector.hh"

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

BDSBeamline* BDS::BuildBLMs(const std::vector<GMAD::BLMPlacement>& blmPlacements,
			    const BDSBeamline* parentBeamLine)
{
  if (blmPlacements.empty())
    {return nullptr;} // don't do anything - no placements

  // we need to loop over all the blm definitions to work out the unique combinations of
  // scorers that need to created. multiple scorers for a single blm ultimately have to be
  // in one G4MultiFunctionalSD sensitive detector.
  std::vector<GMAD::Scorer> scorers = BDSParser::Instance()->GetScorers();
  // convert all the parser scorer definitions into recipes (including parameter checking)
  std::map<G4String, BDSScorerInfo> scorerRecipes;
  for (const auto& scorer : scorers)
    {
      BDSScorerInfo si = BDSScorerInfo(scorer);
      scorerRecipes.insert(std::make_pair(si.name, si));
    }
  
  std::set<std::set<G4String> > scorerSetsToMake;
  for (const auto& bp : blmPlacements)
    {
      std::set<G4String> requiredScorers;
      std::stringstream sqss(bp.scoreQuantity);
      G4String word;
      while (sqss >> word) // split by white space - process word at a time
	{
	  auto search = scorerRecipes.find(word);
	  if (search == scorerRecipes.end())
	    {throw BDSException(__METHOD_NAME__, "scorerQuantity \"" + word + "\" for blm \"" + bp.name + "\" not found.");}
	  requiredScorers.insert(word);
	}
      if (requiredScorers.empty())
	{G4cout << "Warning - no scoreQuantity specified for blm \"" << bp.name << "\" - it will only be passive material" << G4endl;}
      scorerSetsToMake.insert(requiredScorers);
    }
#ifdef BDSDEBUG
  G4cout << "Unqiue combinations of scorers: " << scorerSetsToMake.size() << G4endl;
#endif
  if (scorerSetsToMake.empty())
    {G4cout << "Warning - all BLMs have no scoreQuantity specified so are only passive material." << G4endl;}

  // construct SDs
  BDSScorerFactory scorerFactory;
  std::map<G4String, G4MultiFunctionalDetector*> sensitiveDetectors;
  std::vector<G4String> uniquePrimitiveScorerNames;
  std::vector<G4double> scorerUnits;
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  for (const auto& ss : scorerSetsToMake)
    {
      G4String combinedName = "";
      for (const auto& name : ss) // merge into one name
	{combinedName += name;}

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Making unique SD " << combinedName << G4endl;
#endif
      G4MultiFunctionalDetector* sd = new G4MultiFunctionalDetector("blm_"+combinedName);
      for (const auto& name : ss)
	{
	  auto search = scorerRecipes.find(name);
	  if (search == scorerRecipes.end())
	    {throw BDSException(__METHOD_NAME__, "scorerQuantity \"" + name + "\" not found.");}

	  G4double unit = 1.0;
	  G4VPrimitiveScorer* ps = scorerFactory.CreateScorer(&(search->second), nullptr, &unit);
	  sd->RegisterPrimitive(ps);
	  // We rely on the prefix "blm_" here to intercept scorer hits in BDSOutput so if
	  // this changes, that matching must be done there too. It's to distinguish them
	  // from 3D mesh hits and put them in the BLM output.
	  uniquePrimitiveScorerNames.push_back("blm_"+combinedName+"/"+name);
	  scorerUnits.push_back(unit);
	}
      sensitiveDetectors[combinedName] = sd;
      SDMan->AddNewDetector(sd);
    }
  BDSSDManager::Instance()->RegisterPrimitiveScorerNames(uniquePrimitiveScorerNames, &scorerUnits);
  
  BDSBeamline* blms = new BDSBeamline();
  BDSBLMFactory factory;
  for (const auto& bp : blmPlacements)
    {
      // form a set of score quantities again
      std::set<G4String> requiredScorers;
      std::stringstream sqss(bp.scoreQuantity);
      G4String word;
      while (sqss >> word)
	{requiredScorers.insert(word);}
      G4String combinedName = "";
      for (const auto& name : requiredScorers)
	{combinedName += name;}

      auto sdSearch = sensitiveDetectors.find(combinedName);
      if (sdSearch == sensitiveDetectors.end())
	{throw BDSException(__METHOD_NAME__, "unknown set of scorers");}
      G4MultiFunctionalDetector* sd = sdSearch->second;      

      BDSBLM* blm = factory.BuildBLM(bp.name,
				     bp.geometryFile,
				     bp.geometryType,
				     bp.blmMaterial,
				     bp.blm1 * CLHEP::m,
				     bp.blm2 * CLHEP::m,
				     bp.blm3 * CLHEP::m,
				     bp.blm4 * CLHEP::m,
				     sd);
      
      G4double length = blm->GetExtent().DZ();
      BDSSimpleComponent* comp = new BDSSimpleComponent(blm->GetName(),
							blm,
							length);

      G4double S = -1000;
      G4Transform3D transform = BDSDetectorConstruction::CreatePlacementTransform(bp, parentBeamLine, &S);
      // do a little checking here as transform code in CreatePlacement can't know who's calling it to warn
      if (BDS::IsFinite(bp.s) && transform == G4Transform3D::Identity)
	{throw BDSException(__METHOD_NAME__, "the S coordinate (" + std::to_string(bp.s) + " m) for blm \"" + bp.name + "\" was not found in the beam line - please check");}
      
      BDSBLMRegistry::Instance()->RegisterBLM(bp.name, blm, S);
      
      /// Here we're assuming the length is along z which may not be true, but
      /// close enough for this purpose as we rely only on the centre position.
      G4ThreeVector halfLengthBeg = G4ThreeVector(0,0,-length*0.5);
      G4ThreeVector halfLengthEnd = G4ThreeVector(0,0, length*0.5);
      G4ThreeVector midPos        = transform.getTranslation();
      G4ThreeVector startPos = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthBeg;
      G4ThreeVector endPos   = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthEnd;
      G4RotationMatrix* rm   = new G4RotationMatrix(transform.getRotation());
      
      BDSBeamlineElement* el = new BDSBeamlineElement(comp,
						      startPos,
						      midPos,
						      endPos,
						      rm,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      startPos,
						      midPos,
						      endPos,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      -1,-1,-1);

      blms->AddBeamlineElement(el);
    }

  return blms;
}
