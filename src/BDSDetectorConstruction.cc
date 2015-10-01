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
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSTunnelSD.hh"
#include "BDSTunnelType.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Electron.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4Positron.hh"
#include "G4ProductionCuts.hh"
#include "G4PropagatorInField.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#ifdef BDSDEBUG
bool debug = true;
#else
bool debug = false;
#endif

namespace GMAD {
  extern FastList<Element> beamline_list;
}

typedef std::vector<G4LogicalVolume*>::iterator BDSLVIterator;

BDSDetectorConstruction::BDSDetectorConstruction():
  precisionRegion(nullptr),gasRegion(nullptr),
  worldPV(nullptr),magField(nullptr),
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
  
  // prepare materials for this run
  BDSMaterials::Instance()->PrepareRequiredMaterials();

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

  // implement bias operations on all volumes 
  BuildPhysicsBias();

  // free the parser list - an extern
  GMAD::beamline_list.erase();
  
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
  delete precisionRegion;

  // glash stuff
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
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("gamma"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e-"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e+"));
  gasRegion->SetProductionCuts(theGasProductionCuts);

  // precision region
  precisionRegion = new G4Region("precisionRegion");
  G4ProductionCuts* theProductionCuts = new G4ProductionCuts();
  theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsP(),"gamma");
  theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsP(),"e-");
  theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsP(),"e+");
  precisionRegion->SetProductionCuts(theProductionCuts);
}

void BDSDetectorConstruction::BuildBeamline()
{
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();
  BDSBeamline*         beamline            = new BDSBeamline();

  const BDSExecOptions* execOptions = BDSExecOptions::Instance();
  // Write survey file here since has access to both element and beamline
  BDSSurvey* survey = nullptr;
  if(execOptions->GetSurvey()) {
    survey = new BDSSurvey(execOptions->GetSurveyFilename());
    survey->WriteHeader();
  }
  
  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  for(auto element : GMAD::beamline_list)
    {
#ifdef BDSDEBUG
      G4cout << "BDSDetectorConstruction creating component " << (element).name << G4endl;
#endif
      
      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(element);
      if(temp)
	{
	  BDSTiltOffset* tiltOffset = theComponentFactory->CreateTiltOffset(element);
	  std::vector<BDSBeamlineElement*> addedComponents = beamline->AddComponent(temp, tiltOffset);
	  if (survey) survey->Write(addedComponents, element);
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
      BDS::CalculateAndSetTeleporterDelta(beamline);
      BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator();
      if (terminator)
        {
	  terminator->Initialise();
	  std::vector<BDSBeamlineElement*> addedComponents = beamline->AddComponent(terminator);
	  if (survey) {
	    GMAD::Element element = GMAD::Element(); // dummy element
	    survey->Write(addedComponents, element);
	  }
	}
      BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter();
      if (teleporter)
	{
	  teleporter->Initialise();
	  std::vector<BDSBeamlineElement*> addedComponents = beamline->AddComponent(teleporter);
	  if (survey) {
	    GMAD::Element element = GMAD::Element(); // dummy element
	    survey->Write(addedComponents, element);
	  }
	}
    }

  if (survey) {
    survey->WriteSummary(beamline);
    delete survey;
  }
  delete theComponentFactory;
      
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of the parser beamline element list: "<< GMAD::beamline_list.size() << G4endl;
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
  G4UserLimits* worldUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
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

  BDSBeamline::iterator it = beamline->begin();
  for(; it != beamline->end(); ++it)
    {
      BDSAcceleratorComponent* thecurrentitem = (*it)->GetAcceleratorComponent();
      // do a few checks to see everything's valid before dodgy placement could happen
      if (!thecurrentitem)
	{G4cerr << __METHOD_NAME__ << "beamline element does not contain valid BDSAcceleratorComponent" << G4endl; exit(1);}
      
      // check we can get the container logical volume to be placed
      G4LogicalVolume* elementLV = thecurrentitem->GetContainerLogicalVolume();
      if (!elementLV)
	{G4cerr << __METHOD_NAME__ << "this accelerator component " << (*it)->GetName() << " has no volume to be placed!" << G4endl;  exit(1);}

      // get the name -> note this is the plain name without _pv or _lv suffix just now
      // comes from BDSAcceleratorComponent
      // this is done after the checks as it really just passes down to acc component
      G4String name = (*it)->GetName(); 
      if (verbose || debug)
	{G4cout << __METHOD_NAME__ << "placement of component named: " << name << G4endl;}
      
      // read out geometry logical volume - note may not exist for each item - must be tested
      G4LogicalVolume* readOutLV   = thecurrentitem->GetReadOutLogicalVolume();
      // make read out geometry sensitive
      if (readOutLV)       
	{readOutLV->SetSensitiveDetector(energyCounterSDRO);}
      
      // add the volume to one of the regions
      G4int precision = thecurrentitem->GetPrecisionRegion();
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
      std::vector<G4LogicalVolume*> SensVols = thecurrentitem->GetAllSensitiveVolumes();
      BDSLVIterator sensIt= SensVols.begin();
      for(;sensIt != SensVols.end(); ++sensIt)
	{
	  // use already defined instance of Ecounter sd
	  // we MUST attach this SD to each volume so that it produces
	  // hits (using the read out geometry)
	  (*sensIt)->SetSensitiveDetector(energyCounterSDRO);
	  
	  //set gflash parameterisation on volume if required
	  G4bool gflash     = BDSExecOptions::Instance()->GetGFlash();
	  //TBC - so glash is only used for 'element' types - perhaps this should be used
	  //for other volumes too.  The logic of the if statement needs checked.
	  //The check of the precision region really compares the region pointer of the
	  //logical volume with that of our 'precision region' region. Unclear what the default
	  //region value is in geant4 but it's not our region - no region by default.
	  if(gflash && ((*sensIt)->GetRegion() != precisionRegion) && (thecurrentitem->GetType()=="element"))
	    {SetGFlashOnVolume(*sensIt);}
	}

      // get the placement details from the beamline component
      G4int nCopy          = thecurrentitem->GetCopyNumber();
      // reference rotation and position for the read out volume
      G4ThreeVector     rp = (*it)->GetReferencePositionMiddle();
      G4Transform3D*    pt = (*it)->GetPlacementTransform();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placing mass geometry" << G4endl;
      G4cout << "placement transform position: " << pt->getTranslation()  << G4endl;
      G4cout << "placement transform rotation: " << pt->getRotation()  << G4endl; 
#endif
      
      G4PVPlacement* elementPV = new G4PVPlacement(*pt,                               // placement transform
						   (*it)->GetPlacementName() + "_pv", // name
						   elementLV,                         // logical volume
						   worldPV,                           // mother  volume
						   false,	                      // no boolean operation
						   nCopy,                             // copy number
						   checkOverlaps);                    // overlap checking
      
      // place read out volume in read out world - if this component has one
      G4PVPlacement* readOutPV = nullptr;
      if(readOutLV)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "placing readout geometry" << G4endl;
#endif
	  G4String readOutPVName = name + "_ro_pv";
	  G4Transform3D* ropt = (*it)->GetReadOutPlacementTransform();
	  readOutPV = new G4PVPlacement(*ropt,                                  // placement transform
					(*it)->GetPlacementName() + "_ro_pv", // name
					readOutLV,                            // logical volume
					readOutWorldPV,                       // mother  volume
					false,	                              // no boolean operation
					nCopy,                                // copy number
					checkOverlaps);                       // overlap checking
	  
	  // Register the spos and other info of this elemnet.
	  // Used by energy counter sd to get spos of that logical volume at histogram time.
	  // If it has a readout volume, that'll be used for sensitivity so only need to register
	  // that. Should only register what we need to as used for every energy hit (many many many)
	  
	  // use the readOutLV name as this is what's accessed in BDSEnergyCounterSD
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								     readOutPVName,
								     (*it)->GetSPositionMiddle(),
								     thecurrentitem->GetPrecisionRegion());
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(readOutPV, theinfo, true); // true = it's a read out volume
	}
      /*
      else
        {
	  
	  // It doesn't have a read out volume, so register the same info with all logical volumes
	  // the current BDSAcceleratorComponent  contains as any of them could be requested
	  // by BDSEnergyCounterSD
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								     name,
								     (*it)->GetSPositionMiddle());
	  BDSPVIterator elementLVIterator = thecurrentitem->GetAllLogicalVolumes().begin();
	  BDSPVIterator elementLVEnd      = thecurrentitem->GetAllLogicalVolumes().end();
	  for (; elementLVIterator != elementLVEnd; ++elementLVIterator)
	    {BDSLogicalVolumeInfoRegistry::Instance()->RegisterInfo(*elementLVIterator, theinfo);}
	}
      */
      
      //this does nothing by default - only used by BDSElement
      //looks like it could just be done in its construction rather than
      //in BDSDetectorConstruction
      thecurrentitem->PrepareField(elementPV);
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
      BDSBeamline::iterator tunnelIt          = tunnel->begin();
      for(; tunnelIt != tunnel->end(); ++tunnelIt)
	{
	  BDSAcceleratorComponent* thecurrentitem = (*tunnelIt)->GetAcceleratorComponent();
	  G4LogicalVolume* readOutLV = thecurrentitem->GetReadOutLogicalVolume();
	  if (readOutLV)
	    {readOutLV->SetSensitiveDetector(tunnelSDRO);}
	  auto sensVols = thecurrentitem->GetAllSensitiveVolumes();
	  for(auto sensIt = sensVols.begin(); sensIt != sensVols.end(); ++sensIt)
	    {(*sensIt)->SetSensitiveDetector(tunnelSDRO);}
	  
	  new G4PVPlacement(*(*tunnelIt)->GetPlacementTransform(),    // placement transform
			    (*tunnelIt)->GetPlacementName() + "_pv",  // placement name
			    (*tunnelIt)->GetContainerLogicalVolume(), // volume to be placed
			    worldPV,                                  // volume to place it in
			    false,                                    // no boolean operation
			    0,                                        // copy number
			    checkOverlaps);                           // overlap checking
	  
	  G4String tunnelReadOutPVName = (*tunnelIt)->GetPlacementName() + "_ro_pv";
	  G4PVPlacement* tunnelReadOutPV = new G4PVPlacement(*(*tunnelIt)->GetPlacementTransform(),   // placement transform
							     tunnelReadOutPVName,                     // placement name
							     readOutLV,                               // volume to be placed
							     tunnelReadOutWorldPV,                    // volume to place it in
							     false,                                   // no boolean operation
							     0,                                       // copy number
							     checkOverlaps);                          // overlap checking
	  
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo((*tunnelIt)->GetName(),             // pure name
								     tunnelReadOutPVName,                // read out physical volume name
								     (*tunnelIt)->GetSPositionMiddle()); // s position in middle
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(tunnelReadOutPV, theinfo, true, true);
	  // true,true = it's a read out & tunnel. First true (read out) ignore for tunnel - all read out
	}
    }
  
  // set precision back
  G4cout.precision(G4precision);
}

void BDSDetectorConstruction::BuildPhysicsBias() 
{
#if G4VERSION_NUMBER > 1009

  BDSAcceleratorComponentRegistry* registry = BDSAcceleratorComponentRegistry::Instance();
  // registry is a map, so iterator has first and second members for key and value respectively
  BDSAcceleratorComponentRegistry::iterator i;

  // loop over xsec biases and find if any apply globally 
  // BDSBOptrMultiParticleChangeCrossSection* vacuumBias   = nullptr;
  // BDSBOptrMultiParticleChangeCrossSection* materialBias = nullptr;
  // BDSBOptrMultiParticleChangeCrossSection* tunnelBias   = nullptr;
  
 
  // apply biases
  for (i = registry->begin(); i != registry->end(); ++i)
    {    
      // Accelerator vacuum 
      G4LogicalVolume* vacuumLV = i->second->GetAcceleratorVacuumLogicalVolume();
      if(vacuumLV) 
	{
	  BDSBOptrMultiParticleChangeCrossSection *eg = new BDSBOptrMultiParticleChangeCrossSection();      
	  eg->AddParticle("proton");
	  eg->AttachTo(vacuumLV);
	}

      // Accelerator material
      auto lvs = i->second->GetAllLogicalVolumes();
      for (auto lvsi = lvs.begin(); lvsi != lvs.end(); ++lvsi)
	{
	  BDSBOptrMultiParticleChangeCrossSection *eg = new BDSBOptrMultiParticleChangeCrossSection();
	  eg->AddParticle("e-");
	  eg->AddParticle("e+"); 
	  eg->AddParticle("gamma");
	  eg->AddParticle("proton");
	  eg->AttachTo(*lvsi);
	}
    }  

  // Second for tunnel

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
  //gFlashRegion.back()->SetUserLimits(new G4UserLimits(thecurrentitem->GetChordLength()/10.0));
  //volume->SetUserLimits(new G4UserLimits(thecurrentitem->GetChordLength()/10.0));

}
