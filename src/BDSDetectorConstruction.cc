#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineEndPieceBuilder.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSBeamlineSet.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "BDSComponentFactory.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldObjects.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"
#include "BDSSDManager.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"
#include "parser/physicsbiasing.h"
#include "parser/placement.h"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <vector>

BDSDetectorConstruction::BDSDetectorConstruction():
  placementBL(nullptr),
  brho(std::numeric_limits<double>::max())
{  
  verbose       = BDSGlobalConstants::Instance()->Verbose();
  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
  circular      = BDSGlobalConstants::Instance()->Circular();
  
  // instantiate the accelerator model holding class
  acceleratorModel = BDSAcceleratorModel::Instance();
}

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  if (verbose || debug) G4cout << __METHOD_NAME__ << "starting accelerator geometry construction\n" << G4endl;
  
  // construct regions
  InitialiseRegions();
  
  // construct the component list
  BuildBeamlines();

  // construct placement geometry from parser
  placementBL = BDS::BuildPlacementGeometry(BDSParser::Instance()->GetPlacements());
  BDSAcceleratorModel::Instance()->RegisterPlacementBeamline(placementBL); // Acc model owns it
  
  // build the tunnel and supports
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {BuildTunnel();}

  // build world and calculate coordinates
  auto worldPV = BuildWorld();

  // placement procedure
  ComponentPlacement(worldPV);
  
  if(verbose || debug) G4cout << __METHOD_NAME__ << "detector Construction done"<<G4endl; 

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
  if(verbose || debug) {G4cout << "Finished listing materials, returning physiWorld" << G4endl;} 
#endif
  return worldPV;
}

BDSDetectorConstruction::~BDSDetectorConstruction()
{
#if G4VERSION_NUMBER > 1009
  // delete bias objects
  for (auto i : biasObjects)
    {delete i;}
#endif
}

void BDSDetectorConstruction::InitialiseRegions()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (const GMAD::Region& r : BDSParser::Instance()->GetRegions())
    {
      G4Region* region = new G4Region(G4String(r.name));
      G4ProductionCuts* cuts = new G4ProductionCuts();
      cuts->SetProductionCut(r.prodCutPhotons*CLHEP::m,   "gamma");
      cuts->SetProductionCut(r.prodCutElectrons*CLHEP::m, "e-");
      cuts->SetProductionCut(r.prodCutPositrons*CLHEP::m, "e+");
      cuts->SetProductionCut(r.prodCutProtons*CLHEP::m,   "proton");
      region->SetProductionCuts(cuts);
      acceleratorModel->RegisterRegion(region, cuts);
    }
}

void BDSDetectorConstruction::BuildBeamlines()
{
  // build main beam line
  if (verbose || debug)
    {G4cout << "parsing the beamline element list..."<< G4endl;}
  G4Transform3D initialTransform = BDSGlobalConstants::Instance()->BeamlineTransform();

  BDSBeamlineSet mainBeamline = BuildBeamline(BDSParser::Instance()->GetBeamline(),
					      "main beam line",
					      initialTransform,
					      circular);
  
  if (mainBeamline.massWorld->empty())
    {
      G4cerr << __METHOD_NAME__ << "BDSIM requires the sequence defined with the use command "
	     << "to have at least one element" << G4endl;
      exit(1);
    }

  // register the beamline in the holder class for the full model
  acceleratorModel->RegisterBeamlineSetMain(mainBeamline);

  // loop over placements and check if any are beam lines (have sequences specified)
  auto placements = BDSParser::Instance()->GetPlacements();
  for (const auto& placement : placements)
    {
      if (placement.sequence.empty())
	{continue;} // no sequence specified -> just a placement
      auto parserLine = BDSParser::Instance()->GetSequence(placement.sequence);

      // determine offset in world for extra beam line
      G4Transform3D plTransform = CreatePlacementTransform(placement);
      G4Transform3D placementInitial;
      if (placement.referenceElement.empty())
	{// no reference - ie w.r.t. start of main beam line in the world volume
	  placementInitial = initialTransform; // same as beginning of beam line
	}
      else
	{// use reference to get placement transform - ie w.r.t. to a main beam line element
	  const BDSBeamline* mbl = mainBeamline.massWorld;
	  placementInitial = mbl->GetTransformForElement(placement.referenceElement,
							 placement.referenceElementNumber);
	}
      G4Transform3D startTransform = placementInitial * plTransform; // compound

      // aux beam line must be non-circular by definition to branch off of beam line (for now)
      BDSBeamlineSet extraBeamline = BuildBeamline(parserLine,
						   placement.sequence,
						   startTransform);
      
      acceleratorModel->RegisterBeamlineSetExtra(placement.sequence, extraBeamline);
    }
}

BDSBeamlineSet BDSDetectorConstruction::BuildBeamline(const GMAD::FastList<GMAD::Element>& beamLine,
						      G4String             name,
						      const G4Transform3D& initialTransform,
						      G4bool               beamlineIsCircular)
{
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory(brho);
  BDSBeamline* massWorld = new BDSBeamline(initialTransform);
    
  if (beamlineIsCircular)
    {
      G4bool unsuitable = UnsuitableFirstElement(beamLine.begin());
      if (unsuitable)
	{
	  G4cerr << "The first element in the beam line is unsuitable for a circular "
		 << "model as the first element will " << G4endl << "overlap with the "
		 << "teleporter and terminator - the necessary mechanics for a circular "
		 << "model in Geant4" << G4endl;
	  exit(1);
	}
    }  

  for(auto elementIt = beamLine.begin(); elementIt != beamLine.end(); ++elementIt)
    {
      // find next and previous element, but ignore special elements or thin multipoles.
      const GMAD::Element* prevElement = nullptr;
      auto prevIt = elementIt;
      while (prevIt != beamLine.begin())
	{
	  --prevIt;
	  if (prevIt->isSpecial() == false && prevIt->type != GMAD::ElementType::_THINMULT)
	    {
	      prevElement = &(*prevIt);
	      break;
	    }
	}

      const GMAD::Element* nextElement = nullptr;
      auto nextIt = elementIt;
      ++nextIt;
      while (nextIt != beamLine.end())
	{
	  if (nextIt->isSpecial() == false && nextIt->type != GMAD::ElementType::_THINMULT)
	    {
	      nextElement = &(*nextIt);
	      break;
	    }
	  ++nextIt;
	}
      G4double currentArcLength = massWorld->GetTotalArcLength();
      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(&(*elementIt),
									   prevElement,
									   nextElement,
									   currentArcLength);
      if(temp)
	{
          BDSSamplerType sType = BDS::DetermineSamplerType((*elementIt).samplerType);
          BDSTiltOffset* tiltOffset = theComponentFactory->CreateTiltOffset(&(*elementIt));
          massWorld->AddComponent(temp, tiltOffset, sType, elementIt->samplerName);
	}
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic
  // Add teleporter to account for slight ring offset
  if (beamlineIsCircular)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      // minimum space for the circular mechanics are:
      // 1x terminator with sampler chord length
      // 1x teleporter with (minimum) 1x sampler chord length
      // 3x padding length
      const G4double sL = BDSSamplerPlane::ChordLength();
      const G4double pL = massWorld->PaddingLength();
      G4double minimumRequiredSpace = 2*sL + 3*pL;
      G4ThreeVector teleporterDelta = BDS::CalculateTeleporterDelta(massWorld);
      
      // note delta is from end to beginning, which will have correct transverse but opposite
      // z component, hence -ve here.
      G4double rawLength        = -teleporterDelta.z();
      G4double teleporterLength =  rawLength - minimumRequiredSpace + sL; // leaves at least 1x sL for teleporter
      
      if (teleporterDelta.mag() > 1*CLHEP::m)
	{
	  G4cout << G4endl << "Error - the calculated teleporter delta is above 1m! "
		 << "The teleporter" << G4endl << "was only intended for small shifts "
		 << "- the teleporter will not be built." << G4endl << G4endl;
	}
      else if (teleporterLength < minimumRequiredSpace)
	{// should protect against -ve length teleporter
	  G4cout << G4endl << "Insufficient space between the first and last elements "
		 << "in the beam line" << G4endl << "to fit the terminator and teleporter "
		 << "- these will not be built." << G4endl << G4endl;
	}
      else
	{ 
	  BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator();
	  if (terminator)
	    {
	      terminator->Initialise();
	      massWorld->AddComponent(terminator);
	    }	  
	  // update delta
	  teleporterDelta.setZ(teleporterLength);
	  BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter(teleporterDelta);
	  if (teleporter)
	    {
	      teleporter->Initialise();
	      massWorld->AddComponent(teleporter);
	    }
	}
    }

  if (BDSGlobalConstants::Instance()->Survey())
    {
      BDSSurvey* survey = new BDSSurvey(BDSGlobalConstants::Instance()->SurveyFileName() + ".dat");
      survey->Write(massWorld);
      delete survey;
    }
  delete theComponentFactory;

  // print summary
  G4cout << __METHOD_NAME__ << "\"" << name << "\" " << G4endl << *massWorld;

  // Build curvilinear geometry w.r.t. beam line.
  BDSCurvilinearBuilder* clBuilder = new BDSCurvilinearBuilder();
  BDSBeamline* clBeamline = clBuilder->BuildCurvilinearBeamLine1To1(massWorld, beamlineIsCircular);
  BDSBeamline* clBridgeBeamline = clBuilder->BuildCurvilinearBridgeBeamLine(clBeamline);
  delete clBuilder;

  // construct beamline of end pieces
  BDSBeamline* endPieces = BDS::BuildEndPieceBeamline(massWorld, circular);

  BDSBeamlineSet beamlineSet;
  beamlineSet.massWorld              = massWorld;
  beamlineSet.curvilinearWorld       = clBeamline;
  beamlineSet.curvilinearBridgeWorld = clBridgeBeamline;
  beamlineSet.endPieces              = endPieces;
  return beamlineSet;
}

void BDSDetectorConstruction::BuildTunnel()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  const BDSBeamline* mainBeamLine = acceleratorModel->BeamlineMain();
  BDSBeamline* tunnelBeamline;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder();
  tunnelBeamline = tb->BuildTunnelSections(mainBeamLine);
  delete tb;
  
  acceleratorModel->RegisterTunnelBeamline(tunnelBeamline);
}

G4VPhysicalVolume* BDSDetectorConstruction::BuildWorld()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  std::vector<G4ThreeVector> extents;

  // These beamlines should always exist so are safe to access.
  const auto& blMain = acceleratorModel->BeamlineSetMain();
  extents.push_back(blMain.massWorld->GetMaximumExtentAbsolute());
  const auto blMainCL = blMain.curvilinearWorld;
  if (blMainCL)
    {extents.push_back(blMainCL->GetMaximumExtentAbsolute());}
  
  BDSBeamline* plBeamline = acceleratorModel->PlacementBeamline();
  if (plBeamline) // optional placements beam line
    {extents.push_back(plBeamline->GetMaximumExtentAbsolute());}
  
  BDSBeamline* tunnelBeamline = acceleratorModel->GetTunnelBeamline();
  if (tunnelBeamline)
    {extents.push_back(tunnelBeamline->GetMaximumExtentAbsolute());}

  // Expand to maximum extents of each beam line.
  G4ThreeVector worldR;

  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  for (const auto& bl : extras)
    {// extras is a map, so iterator has first and second for key and value
      extents.push_back(bl.second.massWorld->GetMaximumExtentAbsolute());
      if (bl.second.curvilinearWorld)
	{extents.push_back(bl.second.curvilinearWorld->GetMaximumExtentAbsolute());}
      if (bl.second.curvilinearBridgeWorld)
	{extents.push_back(bl.second.curvilinearBridgeWorld->GetMaximumExtentAbsolute());}
      if (bl.second.endPieces)
	{extents.push_back(bl.second.endPieces->GetMaximumExtentAbsolute());}
    }

  // loop over all extents from all beam lines
  for (const auto& ext : extents)
    {
      for (G4int i = 0; i < 3; i++)
	{worldR[i] = std::max(worldR[i], ext[i]);} // expand with the maximum
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "world extent absolute: " << worldR      << G4endl;
#endif
  worldR += G4ThreeVector(5000,5000,5000); //add 5m extra in every dimension
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "with 5m margin, it becomes in all dimensions: " << worldR << G4endl;
#endif
  
  G4String worldName   = "World";
  worldExtent          = BDSExtent(worldR);
  G4VSolid* worldSolid = new G4Box(worldName + "_solid", worldR.x(), worldR.y(), worldR.z());

  G4String    emptyMaterialName = BDSGlobalConstants::Instance()->EmptyMaterial();
  G4Material* emptyMaterial     = BDSMaterials::Instance()->GetMaterial(emptyMaterialName);
  G4LogicalVolume* worldLV      = new G4LogicalVolume(worldSolid,              // solid
						      emptyMaterial,           // material
						      worldName + "_lv");      // name
  
  // visual attributes
  // copy the debug vis attributes but change to force wireframe
  G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetContainerVisAttr()));
  debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(debugWorldVis);
	
  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->GetDefaultUserLimits());

  // place the world
  G4VPhysicalVolume* worldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						 (G4ThreeVector)0,     // at (0,0,0)
						 worldLV,	            // its logical volume
						 worldName,            // its name
						 nullptr,		    // its mother  volume
						 false,		    // no boolean operation
						 0,                    // copy number
						 checkOverlaps);       // overlap checking

  // Register the lv & pvs to the our holder class for the model
  acceleratorModel->RegisterWorldPV(worldPV);
  acceleratorModel->RegisterWorldLV(worldLV);
  acceleratorModel->RegisterWorldSolid(worldSolid);

  // Register world PV with our auxiliary navigator so steppers and magnetic
  // fields know which geometry to navigate to get local / global transforms.
  // This is the regular world used as a backup to the curvilinear world.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(worldPV);

  /// Give the pv info registry a heads up that these volumes don't have info (optimisation).
  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterExcludedPV(worldPV);
  
  return worldPV;
}

void BDSDetectorConstruction::ComponentPlacement(G4VPhysicalVolume* worldPV)
{
  // We musn't place parallel world geometry here - their world is produced by
  // Geant4 at the right time, so we have a separate placement call for them
  BDSBeamlineSet mainBL = BDSAcceleratorModel::Instance()->BeamlineSetMain();
  PlaceBeamlineInWorld(mainBL.massWorld,
		       worldPV, checkOverlaps,
		       BDSSDManager::Instance()->GetEnergyCounterSD(),
		       true);
  PlaceBeamlineInWorld(mainBL.endPieces,
		       worldPV, checkOverlaps,
		       BDSSDManager::Instance()->GetEnergyCounterSD());
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      PlaceBeamlineInWorld(acceleratorModel->GetTunnelBeamline(),
			   worldPV, checkOverlaps,
			   BDSSDManager::Instance()->GetEnergyCounterTunnelSD());
    }
  PlaceBeamlineInWorld(placementBL, worldPV, checkOverlaps);

  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  for (auto const& bl : extras)
    {// extras is map so iterator has first and second for key and value
      PlaceBeamlineInWorld(bl.second.massWorld, worldPV, checkOverlaps);
      PlaceBeamlineInWorld(bl.second.endPieces, worldPV, checkOverlaps);
    }
}

void BDSDetectorConstruction::PlaceBeamlineInWorld(BDSBeamline*          beamline,
						   G4VPhysicalVolume*    containerPV,
						   G4bool                checkOverlaps,
						   G4VSensitiveDetector* sensitiveDetector,
						   G4bool                setRegions,
						   G4bool                registerInfo,
						   G4bool                useCLPlacementTransform)
{
  if (!beamline)
    {return;}
  
  for (auto element : *beamline)
    { 
      if (setRegions)
	{
	  auto accComp = element->GetAcceleratorComponent();
	  const G4String regionName = accComp->GetRegion();
	  if(!regionName.empty()) // ie string is defined so we should attach region
	    {
	      G4Region* region = BDSAcceleratorModel::Instance()->Region(regionName);
	      auto contLV = accComp->GetContainerLogicalVolume();
	      contLV->SetRegion(region);
	      region->AddRootLogicalVolume(contLV);
	    }
	}

      if (sensitiveDetector)
	{element->GetAcceleratorComponent()->SetSensitiveDetector(sensitiveDetector);}
      
      G4String placementName = element->GetPlacementName() + "_pv";
      G4Transform3D* placementTransform = element->GetPlacementTransform();
      if (useCLPlacementTransform)
	{placementTransform = element->GetPlacementTransformCL();}
      auto pv = new G4PVPlacement(*placementTransform,                  // placement transform
				  placementName,                        // placement name
				  element->GetContainerLogicalVolume(), // volume to be placed
				  containerPV,                          // volume to place it in
				  false,                                // no boolean operation
				  element->GetCopyNo(),                 // copy number
				  checkOverlaps);                       // overlap checking

      if (registerInfo)
        {
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(element->GetName(),
								     placementName,
								     element->GetSPositionMiddle(),
								     element->GetIndex());
	  
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(pv, theinfo, true);
        }
    }
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::Placement& placement)
{
  G4ThreeVector translation = G4ThreeVector(placement.x*CLHEP::m,
					    placement.y*CLHEP::m,
					    placement.z*CLHEP::m);
  
  G4RotationMatrix* rm = nullptr;
  if (placement.axisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(placement.axisX,
					 placement.axisY,
					 placement.axisZ);
      rm = new G4RotationMatrix(axis, placement.angle*CLHEP::rad);
    }
  else
    {
      if (BDS::IsFinite(placement.phi)   ||
	  BDS::IsFinite(placement.theta) ||
	  BDS::IsFinite(placement.psi))
	{// only build if finite
	  CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(placement.phi*CLHEP::rad,
							    placement.theta*CLHEP::rad,
							    placement.psi*CLHEP::rad);
	  rm = new G4RotationMatrix(ang);
	}
      else
	{rm = new G4RotationMatrix();}
    }
  G4Transform3D result(*rm, translation);
  return result;
}

#if G4VERSION_NUMBER > 1009
BDSBOptrMultiParticleChangeCrossSection* BDSDetectorConstruction::BuildCrossSectionBias(
 const std::list<std::string>& biasList,
 G4String defaultBias,
 G4String elementName)
{
  // loop over all physics biasing
  BDSBOptrMultiParticleChangeCrossSection* eg = new BDSBOptrMultiParticleChangeCrossSection();

  const auto& biasObjectList = BDSParser::Instance()->GetBiasing();
  for(std::string const & bs : biasList)
    {
      if (bs.empty() && defaultBias.empty())
	{continue;} // no bias specified and no default

      G4String bias;
      if (bs.empty())
	{// no bias but default specified
	  bias = defaultBias;
	}
      else
	{// bias specified - look it up and ignore default
	  bias = bs;
	}
      
      auto result = biasObjectList.find(bias);
      if (result == biasObjectList.end())
	{
	  G4cout << "Error: bias named \"" << bias << "\" not found for element named \""
		 << elementName << "\"" << G4endl;
	  exit(1);
	}
      const GMAD::PhysicsBiasing& pb = *result;
      
      if(debug)
	{G4cout << __METHOD_NAME__ << "bias loop : " << bs << " " << pb.particle << " " << pb.process << G4endl;}
      
      eg->AddParticle(pb.particle);
      
      // loop through all processes
      for(unsigned int p = 0; p < pb.processList.size(); ++p)
	{eg->SetBias(pb.particle,pb.processList[p],pb.factor[p],(G4int)pb.flag[p]);}
    }

  biasObjects.push_back(eg);
  return eg;
}
#endif

void BDSDetectorConstruction::BuildPhysicsBias() 
{
  if(debug) 
    G4cout << __METHOD_NAME__ << G4endl;
#if G4VERSION_NUMBER > 1009

  BDSAcceleratorComponentRegistry* registry = BDSAcceleratorComponentRegistry::Instance();
  if(debug)
    {G4cout << __METHOD_NAME__ << "registry=" << registry << G4endl;}

  G4String defaultBiasVacuum   = BDSParser::Instance()->GetOptions().defaultBiasVacuum;
  G4String defaultBiasMaterial = BDSParser::Instance()->GetOptions().defaultBiasMaterial;

  // apply per element biases
  for (auto const & item : *registry)
  {
    if (debug)
      {G4cout << __METHOD_NAME__ << "component named: " << item.first << G4endl;}
    BDSAcceleratorComponent* accCom = item.second;
    G4String                accName = accCom->GetName();
    
    // Build vacuum bias object based on vacuum bias list in the component
    auto egVacuum = BuildCrossSectionBias(accCom->GetBiasVacuumList(), defaultBiasVacuum, accName);
    auto vacuumLV = accCom->GetAcceleratorVacuumLogicalVolume();
    if(vacuumLV)
      {
	if(debug)
	  {G4cout << __METHOD_NAME__ << "vacuum volume name: " << vacuumLV
		  << " " << vacuumLV->GetName() << G4endl;}
	egVacuum->AttachTo(vacuumLV);
      }
      
    // Build material bias object based on material bias list in the component
    auto egMaterial = BuildCrossSectionBias(accCom->GetBiasMaterialList(), defaultBiasMaterial, accName);
    auto allLVs     = accCom->GetAllLogicalVolumes();
    if(debug)
      {G4cout << __METHOD_NAME__ << "All logical volumes " << allLVs.size() << G4endl;}
    for (auto materialLV : allLVs)
      {
	if(materialLV != vacuumLV)
	  {
	    if(debug)
	      {G4cout << __METHOD_NAME__ << "All logical volumes " << materialLV
		      << " " << (materialLV)->GetName() << G4endl;}
	    egMaterial->AttachTo(materialLV);
	  }
      }
  }
#endif
}

void BDSDetectorConstruction::ConstructSDandField()
{
  auto flds = BDSFieldBuilder::Instance()->CreateAndAttachAll(); // avoid shadowing 'fields'
  acceleratorModel->RegisterFields(flds);
}

G4bool BDSDetectorConstruction::UnsuitableFirstElement(GMAD::FastList<GMAD::Element>::FastListConstIterator element)
{
  // skip past any line elements in parser to find first non-line element
  while ((*element).type == GMAD::ElementType::_LINE)
    {element++;}
  
  if ((*element).type == GMAD::ElementType::_RBEND)
    {return true;}  // unsuitable
  else if (BDS::IsFinite((*element).e1))
    {return true;}  // unsuitable
  else
    {return false;} // suitable
}
