#include "BDSDetectorConstruction.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineEndPieceBuilder.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldObjects.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"
#include "BDSSDManager.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"
#include "parser/physicsbiasing.h"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "globals.hh"

#include <iterator>
#include <list>
#include <map>
#include <vector>

BDSDetectorConstruction::BDSDetectorConstruction():
  worldPV(nullptr)
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
  BuildBeamline();

  // construct beamline of end pieces
  BDS::BuildEndPieceBeamline(circular);

  // construct placement geometry from parser
  BDS::BuildPlacementGeometry();
  
  // build the tunnel and supports
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {BuildTunnel();}

  // build world and calculate coordinates
  BuildWorld();

  // placement procedure
  ComponentPlacement();
  
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

void BDSDetectorConstruction::BuildBeamline()
{ 
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();
  BDSBeamline*         beamline            = new BDSBeamline();
  
  if (verbose || debug)
    {G4cout << "parsing the beamline element list..."<< G4endl;}

  auto beamLine = BDSParser::Instance()->GetBeamline();
  
  if (circular)
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
#ifdef BDSDEBUG
      G4cout << "BDSDetectorConstruction creating component " << elementIt->name << G4endl;
#endif
      // find next and previous element, but ignore special elements or thin multipoles.
      GMAD::Element* prevElement = nullptr;
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

      GMAD::Element* nextElement = nullptr;
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

      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(&(*elementIt), prevElement, nextElement);
      if(temp)
	{
          BDSSamplerType sType = BDS::DetermineSamplerType((*elementIt).samplerType);
          BDSTiltOffset* tiltOffset = theComponentFactory->CreateTiltOffset(&(*elementIt));
          beamline->AddComponent(temp, tiltOffset, sType, elementIt->samplerName);
	}
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic
  // Add teleporter to account for slight ring offset
  if (circular)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      // minimum space for the circular mechanics are:
      // 1x terminator with sampler chord length
      // 1x teleporter with (minimum) 1x sampler chord length
      // 3x padding length
      const G4double sL = BDSSamplerPlane::ChordLength();
      const G4double pL = beamline->PaddingLength();
      G4double minimumRequiredSpace = 2*sL + 3*pL;
      G4ThreeVector teleporterDelta = BDS::CalculateTeleporterDelta(beamline);
      
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
	      beamline->AddComponent(terminator);
	    }	  
	  // update delta
	  teleporterDelta.setZ(teleporterLength);
	  BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter(teleporterDelta);
	  if (teleporter)
	    {
	      teleporter->Initialise();
	      beamline->AddComponent(teleporter);
	    }
	}
    }

  if (BDSGlobalConstants::Instance()->Survey())
    {
      BDSSurvey* survey = new BDSSurvey(BDSGlobalConstants::Instance()->SurveyFileName() + ".dat");
      survey->Write(beamline);
      delete survey;
    }
  delete theComponentFactory;

  // print summary
  G4cout << __METHOD_NAME__ << *beamline;

#ifdef BDSDEBUG
  // print accelerator component registry
  G4cout << *BDSAcceleratorComponentRegistry::Instance();
  beamline->PrintMemoryConsumption();
#endif
 
  if (beamline->empty())
    {
      G4cout << __METHOD_NAME__ << "beamline empty or no line selected! exiting" << G4endl;
      exit(1);
    }

  // Build curvilinear geometry w.r.t. beam line.
  BDSCurvilinearBuilder* clBuilder = new BDSCurvilinearBuilder();
  BDSBeamline* clBeamline = clBuilder->BuildCurvilinearBeamLine1To1(beamline);
  delete clBuilder;
  
  // register the beamline in the holder class for the full model
  acceleratorModel->RegisterFlatBeamline(beamline);
  acceleratorModel->RegisterCurvilinearBeamline(clBeamline);
}

void BDSDetectorConstruction::BuildTunnel()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* flatBeamLine = acceleratorModel->GetFlatBeamline();
  BDSBeamline* tunnelBeamline;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder();
  tunnelBeamline = tb->BuildTunnelSections(flatBeamLine);
  delete tb;
  
  acceleratorModel->RegisterTunnelBeamline(tunnelBeamline);
}

void BDSDetectorConstruction::BuildWorld()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // These beamlines should always exist so are safe to access.
  G4ThreeVector beamlineExtent = acceleratorModel->GetFlatBeamline()->GetMaximumExtentAbsolute();
  G4ThreeVector clExtent = acceleratorModel->GetCurvilinearBeamline()->GetMaximumExtentAbsolute();
  G4ThreeVector plExtent;
  BDSBeamline* plBeamline = acceleratorModel->GetPlacementBeamline();
  if (plBeamline) // optional placements beam line
    {plExtent = plBeamline->GetMaximumExtentAbsolute();}

  G4ThreeVector tunnelExtent = G4ThreeVector(0,0,0);
  BDSBeamline* tunnelBeamline = acceleratorModel->GetTunnelBeamline();
  if (tunnelBeamline)
    {tunnelExtent = tunnelBeamline->GetMaximumExtentAbsolute();}

  // Expand to maximum extents of each beam line.
  G4ThreeVector worldR;
  for (G4int i = 0; i < 3; i++)
    {worldR[i] = std::max(std::max(beamlineExtent[i],plExtent[i]), std::max(clExtent[i], tunnelExtent[i]));}

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
  worldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
			      (G4ThreeVector)0,     // at (0,0,0)
			      worldLV,	            // its logical volume
			      worldName,            // its name
			      nullptr,		    // its mother  volume
			      false,		    // no boolean operation
			      0,                    // copy number
			      checkOverlaps);       // overlap checking

  // Register the lv & pvs to the our holder class for the model
  acceleratorModel->RegisterWorldPV(worldPV);

  // Register world PV with our auxiliary navigator so steppers and magnetic
  // fields know which geometry to navigate to get local / global transforms.
  // This is the regular world used as a backup to the curvilinear world.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(worldPV);

  /// Give the pv info registry a heads up that these volumes don't have info (optimisation).
  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterExcludedPV(worldPV);
}

void BDSDetectorConstruction::ComponentPlacement()
{
  if (verbose || debug)
    {G4cout << G4endl << __METHOD_NAME__ << "- starting placement procedure" << G4endl;}
  int G4precision = G4cout.precision(15);// set default output formats for BDSDetector:

  // 0 - mass world placement
  // 1 - end piece placement
  // 2 - tunnel placement 

  BDSBeamline*      beamline = acceleratorModel->GetFlatBeamline();
  G4VSensitiveDetector* eCSD = BDSSDManager::Instance()->GetEnergyCounterSD();
  G4VSensitiveDetector* tunnelECSD = BDSSDManager::Instance()->GetEnergyCounterTunnelSD();
  
  // 0 - mass world placement
  for(auto element : *beamline)
    {
      BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();
      
      // check we can get the container logical volume to be placed
      G4LogicalVolume* elementLV = accComp->GetContainerLogicalVolume();
      if (!elementLV)
	{G4cerr << __METHOD_NAME__ << "this accelerator component " << element->GetName() << " has no volume to be placed!" << G4endl;  exit(1);}

      // get the name -> note this is the plain name without _pv or _lv suffix just now
      // comes from BDSAcceleratorComponent
      // this is done after the checks as it really just passes down to acc component
      G4String name = element->GetName(); 
      if (verbose || debug)
	{G4cout << __METHOD_NAME__ << "placement of component named: " << name << G4endl;}
      
      // add the volume to one of the regions
      const G4String regionName = accComp->GetRegion();
      if(!regionName.empty()) // ie string is defined so we should attach region
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "element is in the precision region" << G4endl;
#endif
	  G4Region* region = acceleratorModel->Region(regionName);
	  elementLV->SetRegion(region);
	  region->AddRootLogicalVolume(elementLV);
	}

      // Make sensitive volumes sensitive
      accComp->SetSensitiveDetector(eCSD);
      
      // get the placement details from the beamline component
      G4int nCopy       = element->GetCopyNo();
      // reference rotation and position for the read out volume
      G4ThreeVector  rp = element->GetReferencePositionMiddle();
      G4Transform3D* pt = element->GetPlacementTransform();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placing mass geometry" << G4endl;
      G4cout << "placement transform position: " << pt->getTranslation()  << G4endl;
      G4cout << "placement transform rotation: " << pt->getRotation()  << G4endl; 
#endif
      new G4PVPlacement(*pt,              // placement transform
			element->GetPlacementName() + "_pv", // name
			elementLV,        // logical volume
			worldPV,          // mother volume
			false,	     // no boolean operation
			nCopy,            // copy number
			checkOverlaps);   // overlap checking
    }

  // 1 - end piece placmeent
  auto pieces = acceleratorModel->GetEndPieceBeamline();
  for (auto element : *pieces)
    {
      BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();

      // Make sensitive component sensitive
      accComp->SetSensitiveDetector(eCSD);
      
      G4ThreeVector  rp = element->GetReferencePositionMiddle();
      G4Transform3D* pt = element->GetPlacementTransform();
      G4LogicalVolume* elementLV = accComp->GetContainerLogicalVolume();
      G4int nCopy       = element->GetCopyNo();
      new G4PVPlacement(*pt,              // placement transform
			element->GetPlacementName() + "_pv", // name
			elementLV,        // logical volume
			worldPV,          // mother volume
			false,	     // no boolean operation
			nCopy,            // copy number
			checkOverlaps);   // overlap checking
    }

  // 2 - place the tunnel segments & supports if they're built
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      // place supports
      // use iterator from BDSBeamline.hh
      /*
      BDSBeamline* supports = acceleratorModel->GetSupportsBeamline();
      BDSBeamline::iterator supportsIt = supports->begin();
      G4PVPlacement* supportPV = nullptr;
      for(; supportsIt != supports->end(); ++supportsIt)
	{
	  supportPV = new G4PVPlacement((*supportsIt)->GetRotationMiddle(),         // rotation
					(*supportsIt)->GetPositionMiddle(),         // position
					(*supportsIt)->GetPlacementName() + "_pv",  // placement name
					(*supportsIt)->GetContainerLogicalVolume(), // volume to be placed
					worldPV,                                    // volume to place it in
					false,                                      // no boolean operation
					0,                                          // copy number
					checkOverlaps);                             // overlap checking
					}*/
      
      BDSBeamline* tunnel = acceleratorModel->GetTunnelBeamline();
      
      for (auto element : *tunnel)
	{
	  // Make sensitive volumes sensitive
	  element->GetAcceleratorComponent()->SetSensitiveDetector(tunnelECSD);
	  new G4PVPlacement(*element->GetPlacementTransform(),    // placement transform
			    element->GetPlacementName() + "_pv",  // placement name
			    element->GetContainerLogicalVolume(), // volume to be placed
			    worldPV,                              // volume to place it in
			    false,                                // no boolean operation
			    0,                                    // copy number
			    checkOverlaps);                       // overlap checking
	}
    }

  /// Single placement geometry.
  BDSBeamline* placementBL = acceleratorModel->GetPlacementBeamline();
  if (placementBL)
    {
      for (auto element : *placementBL)
	{
	  new G4PVPlacement(*element->GetPlacementTransform(),
			    element->GetPlacementName() + "_pv",
			    element->GetContainerLogicalVolume(),
			    worldPV,
			    false,
			    0,
			    true);
	}
    }
  
  // set precision back
  G4cout.precision(G4precision);
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
  auto fields = BDSFieldBuilder::Instance()->CreateAndAttachAll();
  acceleratorModel->RegisterFields(fields);
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
