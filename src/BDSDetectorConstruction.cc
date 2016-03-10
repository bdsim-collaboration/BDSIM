#include "BDSDetectorConstruction.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldObjects.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerType.hh"
#include "BDSSDManager.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSTunnelSD.hh"
#include "BDSTunnelType.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"

#include "parser/options.h"

#include "G4Box.hh"
#include "G4Electron.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4Positron.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

#include <iterator>
#include <list>
#include <map>
#include <vector>

typedef std::vector<G4LogicalVolume*>::iterator BDSLVIterator;

BDSDetectorConstruction::BDSDetectorConstruction():
  precisionRegion(nullptr),gasRegion(nullptr),
  worldPV(nullptr),worldUserLimits(nullptr),
  theHitMaker(nullptr),theParticleBounds(nullptr)
{  
  verbose       = BDSExecOptions::Instance()->GetVerbose();
  checkOverlaps = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  G4bool gflash = BDSExecOptions::Instance()->GetGFlash();
  if (gflash) InitialiseGFlash();
  BDSAcceleratorModel::Instance(); // instantiate the accelerator model holding class
}

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  if (verbose || debug) G4cout << __METHOD_NAME__ << "starting accelerator geometry construction\n" << G4endl;
  
  // construct regions
  InitialiseRegions();
  
  // construct the component list
  BuildBeamline();

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
  delete precisionRegion;
  delete worldUserLimits;
  
  // gflash stuff
  gFlashRegion.clear();
  delete theHitMaker;
  delete theParticleBounds;
}

void BDSDetectorConstruction::InitialiseRegions()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // does this belong in BDSPhysicsList ??  Regions are required at construction
  // time, but the only other place production cuts are set is in the physics list.

  // gas region
  gasRegion   = new G4Region("gasRegion");
  G4ProductionCuts* theGasProductionCuts = new G4ProductionCuts();
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,"gamma");
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,"e-");
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,"e+");
  gasRegion->SetProductionCuts(theGasProductionCuts);

  // precision region
  precisionRegion = new G4Region("precisionRegion");
  G4ProductionCuts* precisionProductionCuts = new G4ProductionCuts();
  precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsP(),  "gamma");
  precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsP(),"e-");
  precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsP(),"e+");
  precisionProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutProtonsP(),  "proton");
  precisionRegion->SetProductionCuts(precisionProductionCuts);
}

void BDSDetectorConstruction::BuildBeamline()
{
  const BDSExecOptions* execOptions = BDSExecOptions::Instance();
  
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();
  BDSBeamline*         beamline            = new BDSBeamline();
  
  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  //for(auto element : BDSParser::Instance()->GetBeamline())
  auto beamLine = BDSParser::Instance()->GetBeamline();
  for(auto elementIt = beamLine.begin(); elementIt != beamLine.end(); ++elementIt)

    {
#ifdef BDSDEBUG
      G4cout << "BDSDetectorConstruction creating component " << (*elementIt).name << G4endl;
#endif

      // next and previous element, but ignore samplers or other special elements
      GMAD::Element* prevElement = nullptr;
      auto prevIt = elementIt;
      while (prevIt != beamLine.begin())
	{
	  --prevIt;
	  if (prevIt->isSpecial() == false)
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
	  if (nextIt->isSpecial() == false)
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
	  beamline->AddComponent(temp, tiltOffset, sType, (*elementIt).samplerName);
	}
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic
  // Add teleporter to account for slight ring offset
  if (execOptions->GetCircular())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      G4ThreeVector teleporterDetla = BDS::CalculateAndSetTeleporterDelta(beamline);
      BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator();
      if (terminator)
        {
	  terminator->Initialise();
	  beamline->AddComponent(terminator);
	}
      BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter(teleporterDetla);
      if (teleporter)
	{
	  teleporter->Initialise();
	  beamline->AddComponent(teleporter);
	}
    }

  if(execOptions->GetSurvey())
    {
      BDSSurvey* survey = new BDSSurvey(execOptions->GetSurveyFilename());
      survey->Write(beamline);
      delete survey;
    }
  delete theComponentFactory;
      
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of the parser beamline element list: "<< BDSParser::Instance()->GetBeamline().size() << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "size of the constructed beamline: "<< beamline->size() << " with length " << beamline->GetTotalArcLength()/CLHEP::m << " m" << G4endl;

#ifdef BDSDEBUG
  // print accelerator component registry
  G4cout << *BDSAcceleratorComponentRegistry::Instance();
#endif
 
  if (beamline->empty())
    {
      G4cout << __METHOD_NAME__ << "beamline empty or no line selected! exiting" << G4endl;
      exit(1);
    }
#ifdef BDSDEBUG
  beamline->PrintMemoryConsumption();
#endif
  // register the beamline in the holder class for the full model
  BDSAcceleratorModel::Instance()->RegisterFlatBeamline(beamline);
}

void BDSDetectorConstruction::BuildTunnel()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* flatBeamLine = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  BDSBeamline* tunnelBeamline;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder();
  tunnelBeamline = tb->BuildTunnelSections(flatBeamLine);
  delete tb;
  
  BDSAcceleratorModel::Instance()->RegisterTunnelBeamline(tunnelBeamline);
}

void BDSDetectorConstruction::BuildWorld()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* beamline;
  // remember, the tunnel may not exist...
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {beamline = BDSAcceleratorModel::Instance()->GetTunnelBeamline();}
  else
    {beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();}
  G4ThreeVector worldR      = beamline->GetMaximumExtentAbsolute();
  G4ThreeVector maxpositive = beamline->GetMaximumExtentPositive();
  G4ThreeVector maxnegative = beamline->GetMaximumExtentNegative();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "world extent positive: " << maxpositive << G4endl;
  G4cout << __METHOD_NAME__ << "world extent negative: " << maxnegative << G4endl;
  G4cout << __METHOD_NAME__ << "world extent absolute: " << worldR      << G4endl;
#endif
  worldR += G4ThreeVector(5000,5000,5000); //add 5m extra in every dimension
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "with 5m margin, it becomes in all dimensions: " << worldR << G4endl;
#endif
  
  G4String worldName   = "World";
  G4VSolid* worldSolid = new G4Box(worldName + "_solid", worldR.x(), worldR.y(), worldR.z());

  G4String    emptyMaterialName = BDSGlobalConstants::Instance()->GetEmptyMaterial();
  G4Material* emptyMaterial     = BDSMaterials::Instance()->GetMaterial(emptyMaterialName);
  G4LogicalVolume* worldLV      = new G4LogicalVolume(worldSolid,              // solid
						      emptyMaterial,           // material
						      worldName + "_lv");      // name
  
  // read out geometry logical volume
  // note g4logicalvolume has a private copy constructor so we have to repeat everything here annoyingly
  G4LogicalVolume* readOutWorldLV = new G4LogicalVolume(worldSolid,            // solid
							emptyMaterial,         // material
							worldName + "_ro_lv"); // name

  // tunnel read out geometry logical volume
  // note g4logicalvolume has a private copy constructor so we have to repeat everything here annoyingly
  G4LogicalVolume* tunnelReadOutWorldLV = new G4LogicalVolume(worldSolid,                   // solid
							      emptyMaterial,                // material
							      worldName + "_tunnel_ro_lv"); // name
  
  // visual attributes
  if (BDSExecOptions::Instance()->GetVisDebug())
    {
      // copy the debug vis attributes but change to force wireframe
      G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
      debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
      worldLV->SetVisAttributes(debugWorldVis);
      readOutWorldLV->SetVisAttributes(debugWorldVis);
      tunnelReadOutWorldLV->SetVisAttributes(debugWorldVis);
    }
  else
    {
      worldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      readOutWorldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      tunnelReadOutWorldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
    }
	
  // set limits
#ifndef NOUSERLIMITS
  worldUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  worldUserLimits->SetMaxAllowedStep(worldR.z()*0.5);
  worldLV->SetUserLimits(worldUserLimits);
  readOutWorldLV->SetUserLimits(worldUserLimits);
  tunnelReadOutWorldLV->SetUserLimits(worldUserLimits);
#endif

  // place the world
  worldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
			      (G4ThreeVector)0,     // at (0,0,0)
			      worldLV,	            // its logical volume
			      worldName + "_pv",    // its name
			      nullptr,		    // its mother  volume
			      false,		    // no boolean operation
			      0,                    // copy number
			      checkOverlaps);       // overlap checking

  // create the read out geometry world by creating another placement of the world logical volume
  G4PVPlacement* readOutWorldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						    (G4ThreeVector)0,     // at (0,0,0)
						    readOutWorldLV,	  // logical volume
						    "readoutWorld_pv",    // name
						    nullptr,		  // mother  volume
						    false,		  // no boolean operation
						    0,                    // copy number
						    checkOverlaps);       // overlap checking

  // create the tunnel read out geometry world by creating another placement of the world logical volume
  G4PVPlacement* tunnelReadOutWorldPV = new G4PVPlacement((G4RotationMatrix*)0,     // no rotation
							  (G4ThreeVector)0,         // at (0,0,0)
							  tunnelReadOutWorldLV,     // logical volume
							  "tunnel_readout_world_pv",// name
							  nullptr,		    // mother  volume
							  false,		    // no boolean operation
							  0,                        // copy number
							  checkOverlaps);           // overlap checking

  // Register the lv & pvs to the our holder class for the model
  BDSAcceleratorModel::Instance()->RegisterWorldPV(worldPV);
  BDSAcceleratorModel::Instance()->RegisterReadOutWorldPV(readOutWorldPV);
  BDSAcceleratorModel::Instance()->RegisterReadOutWorldLV(readOutWorldLV);
  BDSAcceleratorModel::Instance()->RegisterTunnelReadOutWorldPV(tunnelReadOutWorldPV);
  BDSAcceleratorModel::Instance()->RegisterTunnelReadOutWorldLV(tunnelReadOutWorldLV);

  // Register world PV with our auxiliary navigator so steppers and magnetic
  // fields know which geometry to navigate to get local / global transforms
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(worldPV);
}

void BDSDetectorConstruction::ComponentPlacement()
{
  if (verbose || debug)
    {G4cout << G4endl << __METHOD_NAME__ << "- starting placement procedure" << G4endl;}

  // set default output formats for BDSDetector:
  int G4precision = G4cout.precision(15);

  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  // few general variables that we don't need to get every
  // time in the loop for component placement
  G4VPhysicalVolume* readOutWorldPV       = BDSAcceleratorModel::Instance()->GetReadOutWorldPV();
  G4VSensitiveDetector* energyCounterSDRO = BDSSDManager::Instance()->GetEnergyCounterOnAxisSDRO();
  
  for(auto element : *beamline)
    {
      BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();
      // do a few checks to see everything's valid before dodgy placement could happen
      if (!accComp)
	{G4cerr << __METHOD_NAME__ << "beamline element does not contain valid BDSAcceleratorComponent" << G4endl; exit(1);}
      
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
      
      // read out geometry logical volume - note may not exist for each item - must be tested
      G4LogicalVolume* readOutLV   = accComp->GetReadOutLogicalVolume();
      // make read out geometry sensitive
      if (readOutLV)       
	{readOutLV->SetSensitiveDetector(energyCounterSDRO);}
      
      // add the volume to one of the regions
      G4int precision = accComp->GetPrecisionRegion();
      if(precision > 0)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "element is in the precision region number: " << precision << G4endl;
#endif
	  elementLV->SetRegion(precisionRegion);
	  precisionRegion->AddRootLogicalVolume(elementLV);
	}
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "setting up sensitive volumes with read out geometry" << G4endl;
#endif
      for (auto lv : accComp->GetAllSensitiveVolumes())
	{
	  // Attach this SD to each volume so that it produce hits using read out geometry
	  lv->SetSensitiveDetector(energyCounterSDRO);
	  
	  //set gflash parameterisation on volume if required
	  G4bool gflash     = BDSExecOptions::Instance()->GetGFlash();
	  //TBC - so glash is only used for 'element' types - perhaps this should be used
	  //for other volumes too.  The logic of the if statement needs checked.
	  //The check of the precision region really compares the region pointer of the
	  //logical volume with that of our 'precision region' region. Unclear what the default
	  //region value is in geant4 but it's not our region - no region by default.
	  if(gflash && (lv->GetRegion() != precisionRegion) && (accComp->GetType()=="element"))
	    {SetGFlashOnVolume(lv);}
	}

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
      G4PVPlacement* elementPV = new G4PVPlacement(*pt,              // placement transform
                                                   element->GetPlacementName() + "_pv", // name
						   elementLV,        // logical volume
						   worldPV,          // mother volume
						   false,	     // no boolean operation
						   nCopy,            // copy number
						   checkOverlaps);   // overlap checking
      
      // place read out volume in read out world - if this component has one
      G4PVPlacement* readOutPV = nullptr;
      if(readOutLV)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "placing readout geometry" << G4endl;
#endif
	  G4String readOutPVName = element->GetPlacementName() + "_ro_pv";
	  G4Transform3D* ropt = element->GetReadOutPlacementTransform();
	  readOutPV = new G4PVPlacement(*ropt,          // placement transform
					readOutPVName,  // name
					readOutLV,      // logical volume
					readOutWorldPV, // mother  volume
					false,	        // no boolean operation
					nCopy,          // copy number
					checkOverlaps); // overlap checking
	  
	  // Register the spos and other info of this elemnet.
	  // Used by energy counter sd to get spos of that logical volume at histogram time.
	  // If it has a readout volume, that'll be used for sensitivity so only need to register
	  // that. Should only register what we need to as used for every energy hit (many many many)
	  
	  // use the readOutLV name as this is what's accessed in BDSEnergyCounterSD
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								     readOutPVName,
								     element->GetSPositionMiddle(),
								     accComp->GetPrecisionRegion());

	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(readOutPV, theinfo, true);
	}
      
      //this does nothing by default - only used by BDSElement
      //looks like it could just be done in its construction rather than
      //in BDSDetectorConstruction
      accComp->PrepareField(elementPV);
    }

  // place the tunnel segments & supports if they're built
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      // place supports
      // use iterator from BDSBeamline.hh
      /*
      BDSBeamline* supports = BDSAcceleratorModel::Instance()->GetSupportsBeamline();
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
      
      // place the tunnel
      G4VPhysicalVolume* tunnelReadOutWorldPV = BDSAcceleratorModel::Instance()->GetTunnelReadOutWorldPV();
      G4VSensitiveDetector* tunnelSDRO        = BDSSDManager::Instance()->GetTunnelOnAxisSDRO();
      BDSBeamline* tunnel                     = BDSAcceleratorModel::Instance()->GetTunnelBeamline();
      
      for (auto element : *tunnel)
	{
	  BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();
	  G4LogicalVolume* readOutLV = accComp->GetReadOutLogicalVolume();
	  if (readOutLV)
	    {readOutLV->SetSensitiveDetector(tunnelSDRO);}
	  //auto sensVols = accComp->GetAllSensitiveVolumes();
	  for (auto lv : accComp->GetAllSensitiveVolumes())
	    //	  for(auto sensIt = sensVols.begin(); sensIt != sensVols.end(); ++sensIt)
	    {lv->SetSensitiveDetector(tunnelSDRO);}
	  
	  new G4PVPlacement(*element->GetPlacementTransform(),    // placement transform
			    element->GetPlacementName() + "_pv",  // placement name
			    element->GetContainerLogicalVolume(), // volume to be placed
			    worldPV,                                  // volume to place it in
			    false,                                    // no boolean operation
			    0,                                        // copy number
			    checkOverlaps);                           // overlap checking
	  
	  G4String tunnelReadOutPVName = element->GetPlacementName() + "_ro_pv";
	  G4PVPlacement* tunnelReadOutPV = new G4PVPlacement(*element->GetPlacementTransform(),   // placement transform
							     tunnelReadOutPVName,                     // placement name
							     readOutLV,                               // volume to be placed
							     tunnelReadOutWorldPV,                    // volume to place it in
							     false,                                   // no boolean operation
							     0,                                       // copy number
							     checkOverlaps);                          // overlap checking
	  
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(element->GetName(),             // pure name
								     tunnelReadOutPVName,                // read out physical volume name
								     element->GetSPositionMiddle()); // s position in middle
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(tunnelReadOutPV, theinfo, true, true);
	  // true,true = it's a read out & tunnel. First true (read out) ignore for tunnel - all read out
	}
    }
  
  // set precision back
  G4cout.precision(G4precision);
}

#if G4VERSION_NUMBER > 1009
BDSBOptrMultiParticleChangeCrossSection* BDSDetectorConstruction::BuildCrossSectionBias(
        const std::list<std::string>& biasList)
{
  // loop over all physics biasing
  BDSBOptrMultiParticleChangeCrossSection* eg = new BDSBOptrMultiParticleChangeCrossSection();
  for(std::string const & bs : biasList)
    {
      auto it = BDSParser::Instance()->GetBiasing().find(bs);
      if (it==BDSParser::Instance()->GetBiasing().end()) continue;
      const GMAD::PhysicsBiasing& pb = *it;
      
      if(debug)
	{G4cout << __METHOD_NAME__ << "bias loop : " << bs << " " << pb.particle << " " << pb.process << G4endl;}
      
      eg->AddParticle(pb.particle);
      
      // loop through all processes
      for(unsigned int p = 0; p < pb.processList.size(); ++p)
	{
	  if(debug)
	    {
	      G4cout << __METHOD_NAME__ << "Process loop "
		     << pb.processList[p] << " " << pb.factor[p] << " " << (int)pb.flag[p] << G4endl;
	    }
	  eg->SetBias(pb.particle,pb.processList[p],pb.factor[p],(int)pb.flag[p]);
	}
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

  // apply per element biases
  for (auto const & item : *registry)
  {
    if (debug)
      {G4cout << __METHOD_NAME__ << "component named: " << item.first << G4endl;}
    BDSAcceleratorComponent* accCom = item.second;
    // Build vacuum bias object based on vacuum bias list in the component
    auto egVacuum = BuildCrossSectionBias(accCom->GetBiasVacuumList());
    auto vacuumLV = accCom->GetAcceleratorVacuumLogicalVolume();
    if(vacuumLV)
      {
	if(debug)
	  {G4cout << __METHOD_NAME__ << "vacuum volume name: " << vacuumLV << " " << vacuumLV->GetName() << G4endl;}
	{egVacuum->AttachTo(vacuumLV);}
      }
      
    // Build material bias object based on material bias list in the component
    auto egMaterial = BuildCrossSectionBias(accCom->GetBiasMaterialList());
    auto allLVs     = accCom->GetAllLogicalVolumes();
    if(debug)
      {G4cout << __METHOD_NAME__ << "All logical volumes " << allLVs.size() << G4endl;}
    for (auto materialLV : allLVs)
      {
	if(materialLV != vacuumLV)
	  {
	    if(debug)
	      {G4cout << __METHOD_NAME__ << "All logical volumes " << materialLV << " " << (materialLV)->GetName() << G4endl;}
	    egMaterial->AttachTo(materialLV);
	  }
      }
  }
#endif
}

void BDSDetectorConstruction::InitialiseGFlash()
{
  G4double gflashemax = BDSExecOptions::Instance()->GetGFlashEMax();
  G4double gflashemin = BDSExecOptions::Instance()->GetGFlashEMin();
  theParticleBounds  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  theParticleBounds->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),gflashemax*CLHEP::GeV);
  theParticleBounds->SetMinEneToParametrise(*G4Electron::ElectronDefinition(),gflashemin*CLHEP::GeV);
  // does this break energy conservation??
  //theParticleBounds->SetEneToKill(*G4Electron::ElectronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
      
  theParticleBounds->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),gflashemax*CLHEP::GeV);
  theParticleBounds->SetMinEneToParametrise(*G4Positron::PositronDefinition(),gflashemin*CLHEP::GeV);
  // does this break energy conservation??
  //theParticleBounds->SetEneToKill(*G4Positron::PositronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
      
  // theParticleBoundsVac  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  // theParticleBoundsVac->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),0*CLHEP::GeV);
  // theParticleBoundsVac->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),0*CLHEP::GeV);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "theParticleBounds - min E - electron: " 
	 << theParticleBounds->GetMinEneToParametrise(*G4Electron::ElectronDefinition())/CLHEP::GeV<< " GeV" << G4endl;
  G4cout << __METHOD_NAME__ << "theParticleBounds - max E - electron: " 
	 << theParticleBounds->GetMaxEneToParametrise(*G4Electron::ElectronDefinition())/CLHEP::GeV<< G4endl;
  G4cout << __METHOD_NAME__ << "theParticleBounds - kill E - electron: " 
	 << theParticleBounds->GetEneToKill(*G4Electron::ElectronDefinition())/CLHEP::GeV<< G4endl;
  G4cout << __METHOD_NAME__ << "theParticleBounds - min E - positron: " 
	 << theParticleBounds->GetMinEneToParametrise(*G4Positron::PositronDefinition())/CLHEP::GeV<< G4endl;
  G4cout << __METHOD_NAME__ << "theParticleBounds - max E - positron: " 
	 << theParticleBounds->GetMaxEneToParametrise(*G4Positron::PositronDefinition())/CLHEP::GeV<< G4endl;
  G4cout << __METHOD_NAME__ << "theParticleBounds - kill E - positron: " 
	 << theParticleBounds->GetEneToKill(*G4Positron::PositronDefinition())/CLHEP::GeV<< G4endl;
#endif
  theHitMaker = new GFlashHitMaker();// Makes the EnergySpots 
}

void BDSDetectorConstruction::SetGFlashOnVolume(G4LogicalVolume* volume)
{
  // this has been taken from component placement and put in a separate function to make clearer
  // for now.  perhaps should be revisited. LN

  //If not in the precision region....
  //		    if(volume->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
#ifdef BDSDEBUG
  G4cout << "...adding " << volume->GetName() << " to gFlashRegion" << G4endl;
#endif
  // Initialise shower model
  G4String rname = "gFlashRegion_" + volume->GetName();
  gFlashRegion.push_back(new G4Region(rname.c_str()));
  G4String mname = "fastShowerModel" + rname;
#ifdef BDSDEBUG
  G4cout << "...making parameterisation..." << G4endl;
#endif
  theFastShowerModel.push_back(new BDSShowerModel(mname.c_str(),gFlashRegion.back()));
  theParameterisation.push_back(new GFlashHomoShowerParameterisation(BDSMaterials::Instance()->GetMaterial(volume->GetMaterial()->GetName().c_str()))); 
  theFastShowerModel.back()->SetParameterisation(*theParameterisation.back());
  theFastShowerModel.back()->SetParticleBounds(*theParticleBounds) ;
  theFastShowerModel.back()->SetHitMaker(*theHitMaker);
  if(volume->GetMaterial()->GetState()!=kStateGas)
    { //If the region material state is not gas, associate with a parameterisation
      //Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
      theFastShowerModel.back()->SetFlagParamType(1);
      //Turn on containment
      theFastShowerModel.back()->SetFlagParticleContainment(1);
    }
  else
    {
      //Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
      theFastShowerModel.back()->SetFlagParamType(0);
      //Turn on containment
      theFastShowerModel.back()->SetFlagParticleContainment(0);
  }
  volume->SetRegion(gFlashRegion.back());
  gFlashRegion.back()->AddRootLogicalVolume(volume);
  //gFlashRegion.back()->SetUserLimits(new G4UserLimits(accComp->GetChordLength()/10.0));
  //volume->SetUserLimits(new G4UserLimits(accComp->GetChordLength()/10.0));

}

void BDSDetectorConstruction::ConstructSDandField()
{
  auto fields = BDSFieldBuilder::Instance()->CreateAndAttachAll();
  BDSAcceleratorModel::Instance()->RegisterFields(fields);
}
