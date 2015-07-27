#include "BDSDetectorConstruction.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSMaterials.hh"
#include "BDSSDManager.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSTunnelType.hh"
#include "BDSTunnelFactory.hh"

#include "parser/element.h"
#include "parser/elementlist.h"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Electron.hh"
#include "G4GeometryManager.hh"
#include "G4GeometrySampler.hh"
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
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSampler.hh"
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

typedef std::vector<G4LogicalVolume*>::iterator BDSLVIterator;

BDSDetectorConstruction::BDSDetectorConstruction():
  precisionRegion(NULL),gasRegion(NULL),
  magField(NULL),
  theHitMaker(NULL),theParticleBounds(NULL)
{  
  verbose       = BDSExecOptions::Instance()->GetVerbose();
  checkOverlaps = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  InitialiseGFlash();
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
    {BuildTunnelAndSupports();}

  // build world and calculate coordinates
  BuildWorld();

  // placement procedure
  ComponentPlacement();

  // free the parser list - an extern
  beamline_list.erase();
  
  if(verbose || debug) G4cout << __METHOD_NAME__ << "detector Construction done"<<G4endl; 

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
  if(verbose || debug) G4cout<<"Finished listing materials, returning physiWorld"<<G4endl; 
  
  G4cout << *BDSPhysicalVolumeInfoRegistry::Instance();
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
  std::list<struct Element>::iterator it;

  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();
  BDSBeamline*         beamline            = new BDSBeamline();

  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  for(it = beamline_list.begin();it!=beamline_list.end();it++)
    {
#ifdef BDSDEBUG
      G4cout << "BDSDetectorConstruction creating component " << (*it).name << G4endl;
#endif
      
      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(*it);
      if(temp)
	{
	  BDSTiltOffset* tiltOffset = theComponentFactory->CreateTiltOffset(*it);
	  beamline->AddComponent(temp, tiltOffset);
	}
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic
  // Add teleporter to account for slight ring offset
  if (BDSExecOptions::Instance()->GetCircular())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      BDS::CalculateAndSetTeleporterDelta(beamline);
      BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator();
      if (terminator)
        {
	  terminator->Initialise();
	  beamline->AddComponent(terminator);
	}
      BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter();
      if (teleporter)
	{
	  teleporter->Initialise();
	  beamline->AddComponent(teleporter);
	}
    }
  
  delete theComponentFactory;
      
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of the parser beamline element list: "<< beamline_list.size() << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "size of the constructed beamline: "<< beamline->size() << " with length " << beamline->GetTotalArcLength()/CLHEP::m << " m" << G4endl;
  
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

void BDSDetectorConstruction::BuildTunnelAndSupports()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* flatBeamLine = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  std::pair<BDSBeamline*, BDSBeamline*> tunnelAndSupports;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder();
  tunnelAndSupports = tb->BuildTunnelAndSupports(flatBeamLine);
  delete tb;

  BDSAcceleratorModel::Instance()->RegisterSupportsBeamline(tunnelAndSupports.first);
  BDSAcceleratorModel::Instance()->RegisterTunnelBeamline(tunnelAndSupports.second);
}

void BDSDetectorConstruction::BuildWorld()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  
  G4ThreeVector worldR = beamline->GetMaximumExtentAbsolute();
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
  
  // visual attributes
  if (BDSExecOptions::Instance()->GetVisDebug())
    {
      // copy the debug vis attributes but change to force wireframe
      G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
      debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
      worldLV->SetVisAttributes(debugWorldVis);
      readOutWorldLV->SetVisAttributes(debugWorldVis);
    }
  else
    {
      worldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      readOutWorldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
    }
	
  // set limits
#ifndef NOUSERLIMITS
  G4UserLimits* worldUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  worldUserLimits->SetMaxAllowedStep(worldR.z()*0.5);
  worldLV->SetUserLimits(worldUserLimits);
  readOutWorldLV->SetUserLimits(worldUserLimits);
#endif

  // place the world
  worldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
			      (G4ThreeVector)0,     // at (0,0,0)
			      worldLV,	            // its logical volume
			      worldName + "_pv",    // its name
			      NULL,		    // its mother  volume
			      false,		    // no boolean operation
			      0,                    // copy number
			      checkOverlaps);       // overlap checking

  // create the read out geometry world by creating another placement of the world logical volume
  G4PVPlacement* readOutWorldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						    (G4ThreeVector)0,     // at (0,0,0)
						    readOutWorldLV,	  // logical volume
						    "readoutWorld_pv",    // name
						    NULL,		  // mother  volume
						    false,		  // no boolean operation
						    0,                    // copy number
						    checkOverlaps);       // overlap checking

  // Register the lv & pvs to the our holder class for the model
  BDSAcceleratorModel::Instance()->RegisterWorldPV(worldPV);
  
  BDSAcceleratorModel::Instance()->RegisterReadOutWorldPV(readOutWorldPV);
  BDSAcceleratorModel::Instance()->RegisterReadOutWorldLV(readOutWorldLV);
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

  BDSBeamlineIterator it = beamline->begin();
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
      G4int nCopy         = 0;
      G4RotationMatrix* r = (*it)->GetRotationMiddle();
      G4ThreeVector     p = (*it)->GetPositionMiddle();
      // reference rotation and position for the read out volume
      G4RotationMatrix* rr = (*it)->GetReferenceRotationMiddle();
      G4ThreeVector     rp = (*it)->GetReferencePositionMiddle();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placing mass geometry" << G4endl;
      G4cout << "position: " << p << ", rotation: " << *r << G4endl;
#endif
      
      G4PVPlacement* elementPV = new G4PVPlacement(r,                                 // its rotation
						   p,                                 // its position
						   (*it)->GetPlacementName() + "_pv", // its name
						   elementLV,                         // its logical volume
						   worldPV,                           // its mother  volume
						   false,	                      // no boolean operation
						   nCopy,                             // copy number
						   checkOverlaps);                    //overlap checking

      // place read out volume in read out world - if this component has one
      G4PVPlacement* readOutPV = NULL;
      if(readOutLV)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "placing readout geometry" << G4endl;
	  G4cout << "position: " << rp << ", rotation: " << *rr << G4endl;
#endif
	  G4String readOutPVName = name + "_ro_pv";
	  // don't need the returned pointer from new for anything - purely instantiating registers it with g4
	  readOutPV = new G4PVPlacement(rr,                                   // its rotation
					rp,                                   // its position
					(*it)->GetPlacementName() + "_ro_pv", // its name
					readOutLV,                            // its logical volume
					readOutWorldPV,                       // its mother  volume
					false,	                              // no boolean operation
					nCopy,                                // copy number
					checkOverlaps);                       //overlap checking

	  // Register the spos and other info of this elemnet.
	  // Used by energy counter sd to get spos of that logical volume at histogram time.
	  // If it has a readout volume, that'll be used for sensitivity so only need to register
	  // that. Should only register what we need to as used for every energy hit (many many many)
	  
	  // use the readOutLV name as this is what's accessed in BDSEnergyCounterSD
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								     readOutPVName,
								     (*it)->GetSPositionMiddle());
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

  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      // place supports
      // use iterator from BDSBeamline.hh
      BDSBeamline* supports = BDSAcceleratorModel::Instance()->GetSupportsBeamline();
      BDSBeamlineIterator supportsIt = supports->begin();
      G4PVPlacement* supportPV = NULL;
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
	}
      
      // place the tunnel
      BDSBeamline* tunnel = BDSAcceleratorModel::Instance()->GetTunnelBeamline();
      BDSBeamlineIterator tunnelIt = tunnel->begin();
      G4PVPlacement* tunnelPV = NULL;
      for(; tunnelIt != tunnel->end(); ++tunnelIt)
	{
	  tunnelPV = new G4PVPlacement((*tunnelIt)->GetRotationMiddle(),         // rotation
				       (*tunnelIt)->GetPositionMiddle(),         // position
				       (*tunnelIt)->GetPlacementName() + "_pv",  // placement name
				       (*tunnelIt)->GetContainerLogicalVolume(), // volume to be placed
				       worldPV,                                  // volume to place it in
				       false,                                    // no boolean operation
				       0,                                        // copy number
				       checkOverlaps);                           // overlap checking
	}
    }
  // set precision back
  G4cout.precision(G4precision);
}

void BDSDetectorConstruction::InitialiseGFlash()
{
  G4bool gflash = BDSExecOptions::Instance()->GetGFlash();
  if (gflash)
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
}

void BDSDetectorConstruction::SetGFlashOnVolume(G4LogicalVolume* volume)
{
  // this has been taken from component placement and put in a separate funciton to make clearer
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
