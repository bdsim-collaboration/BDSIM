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

#include "parser/element.h"
#include "parser/elementlist.h"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Electron.hh"
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
  itsGeometrySampler(NULL),precisionRegion(NULL),gasRegion(NULL),
  solidWorld(NULL),logicWorld(NULL),physiWorld(NULL),
  magField(NULL),BDSUserLimits(NULL),BDSSensitiveDetector(NULL),
  theHitMaker(NULL),theParticleBounds(NULL),_globalRotation(NULL)
{  
  verbose    = BDSExecOptions::Instance()->GetVerbose();

  //initialize global rotation matrix
  _globalRotation = new G4RotationMatrix();

  G4bool gflash = BDSExecOptions::Instance()->GetGFlash();
  if (gflash) {
    G4double gflashemax = BDSExecOptions::Instance()->GetGFlashEMax();
    G4double gflashemin = BDSExecOptions::Instance()->GetGFlashEMin();
    // GFlashStuff
    theParticleBounds  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
    theParticleBounds->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),gflashemax*CLHEP::GeV);
    theParticleBounds->SetMinEneToParametrise(*G4Electron::ElectronDefinition(),gflashemin*CLHEP::GeV);
    theParticleBounds->SetEneToKill(*G4Electron::ElectronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
    
    theParticleBounds->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),gflashemax*CLHEP::GeV);
    theParticleBounds->SetMinEneToParametrise(*G4Positron::PositronDefinition(),gflashemin*CLHEP::GeV);
    theParticleBounds->SetEneToKill(*G4Positron::PositronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
    
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

    theHitMaker          = new GFlashHitMaker();                    // Makes the EnergySpots 
  }
}

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  gasRegion   = new G4Region("gasRegion");

  G4ProductionCuts* theGasProductionCuts = new G4ProductionCuts();
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("gamma"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e-"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e+"));
  gasRegion->SetProductionCuts(theGasProductionCuts);
  
  if (verbose || debug) G4cout << __METHOD_NAME__ << "starting accelerator geometry construction\n" << G4endl;
  
  // prepare materials for this run
  BDSMaterials::Instance()->PrepareRequiredMaterials();
  
  // set global magnetic field first
  SetMagField(0.0); // necessary to set a global field; so choose zero
  
  // construct the component list
  BuildBeamline();

  // build world and calculate coordinates
  BuildWorld();

  // placement procedure
  ComponentPlacement();

  // free the parser list
  beamline_list.erase();
  
  if(verbose || debug) G4cout << __METHOD_NAME__ << "detector Construction done"<<G4endl; 

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
  if(verbose || debug) G4cout<<"Finished listing materials, returning physiWorld"<<G4endl; 
#endif
  
  // feedback
#ifdef BDSDEBUG
  G4cout << *BDSPhysicalVolumeInfoRegistry::Instance();
#endif  
  return physiWorld;
}
 
void BDSDetectorConstruction::SetMagField(const G4double fieldValue){
  
  G4FieldManager* fieldMgr =
    G4TransportationManager::GetTransportationManager()->GetFieldManager();
  magField = new G4UniformMagField(G4ThreeVector(0.,fieldValue,0.));  
  fieldMgr->SetDetectorField(magField);
  fieldMgr->CreateChordFinder(magField);
}

BDSDetectorConstruction::~BDSDetectorConstruction()
{ 
  delete precisionRegion;
  gFlashRegion.clear();

  delete _globalRotation;

  delete theHitMaker;
  delete theParticleBounds;
}

void BDSDetectorConstruction::BuildBeamline()
{
  std::list<struct Element>::iterator it;

  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();

  BDSBeamline* beamline = new BDSBeamline();

  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  for(it = beamline_list.begin();it!=beamline_list.end();it++)
    {
#ifdef BDSDEBUG
      G4cout << "BDSDetectorConstruction creating component " << (*it).name << G4endl;
#endif
      
      BDSAcceleratorComponent* temp = theComponentFactory->createComponent(*it);
      if(temp)
	{
	  BDSTiltOffset* tiltOffset = theComponentFactory->createTiltOffset(*it);
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
      BDSAcceleratorComponent* terminator = theComponentFactory->createTerminator();
      if (terminator)
        {
	  terminator->Initialise();
	  beamline->AddComponent(terminator);
	}
      BDSAcceleratorComponent* teleporter = theComponentFactory->createTeleporter();
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
  // register the beamline in the holder class for the full model
  BDSAcceleratorModel::Instance()->RegisterFlatBeamline(beamline);
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
  G4cout << __METHOD_NAME__ << "with 5m margin becomes in all dimensions: " << worldR << G4endl;
#endif
  
  G4String worldName="World";
  solidWorld = new G4Box(worldName, worldR.x(), worldR.y(), worldR.z());

  G4String    emptyMaterialName = BDSGlobalConstants::Instance()->GetEmptyMaterial();
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(emptyMaterialName);
  logicWorld = new G4LogicalVolume(solidWorld,	       // solid
				   emptyMaterial,      // material
				   worldName);	       // name

  // read out geometry logical volume
  // note g4logicalvolume has a private copy constructor so we have to repeat everything here annoyingly
  G4LogicalVolume* readOutWorldLV = new G4LogicalVolume(solidWorld,    // solid
							emptyMaterial, // material
							worldName);    // name
  
  // visual attributes
  if (BDSExecOptions::Instance()->GetVisDebug())
    {
      // copy the debug vis attributes but change to force wireframe
      G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
      debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
      logicWorld->SetVisAttributes(debugWorldVis);
      readOutWorldLV->SetVisAttributes(debugWorldVis);
    }
  else
    {
      logicWorld->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
      readOutWorldLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
    }
	
  // set limits
#ifndef NOUSERLIMITS
  G4UserLimits* worldUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  worldUserLimits->SetMaxAllowedStep(worldR.z()*0.5);
  logicWorld->SetUserLimits(worldUserLimits);
  readOutWorldLV->SetUserLimits(worldUserLimits);
#endif

  // create regions
#ifdef BDSDEBUG
  G4cout<<"Creating regions..."<<G4endl;
#endif
  precisionRegion = new G4Region("precisionRegion");
  G4ProductionCuts* theProductionCuts = new G4ProductionCuts();
  if(BDSGlobalConstants::Instance()->GetProdCutPhotonsP()>0)
    theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsP(),G4ProductionCuts::GetIndex("gamma"));

  if(BDSGlobalConstants::Instance()->GetProdCutElectronsP()>0)
    theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsP(),G4ProductionCuts::GetIndex("e-"));

  if(BDSGlobalConstants::Instance()->GetProdCutPositronsP()>0)
    theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsP(),G4ProductionCuts::GetIndex("e+"));
  
  precisionRegion->SetProductionCuts(theProductionCuts);
#ifndef NOUSERLIMITS
  precisionRegion->SetUserLimits(worldUserLimits);
#endif

  // place the world
  physiWorld = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
  				 (G4ThreeVector)0,     // at (0,0,0)
                                 logicWorld,	// its logical volume
                                 worldName,	// its name
                                 NULL,		// its mother  volume
                                 false,		// no boolean operation
                                 0,             // copy number
				 BDSGlobalConstants::Instance()->GetCheckOverlaps());// overlap checking

  // create the read out geometry world by creating another placement of the world logical volume
  G4PVPlacement* readOutWorldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						    (G4ThreeVector)0,     // at (0,0,0)
						    readOutWorldLV,	  // logical volume
						    "readoutWorld",       // name
						    NULL,		  // mother  volume
						    false,		  // no boolean operation
						    0,                    // copy number
						    BDSGlobalConstants::Instance()->GetCheckOverlaps());// overlap checking

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
  G4bool checkOverlaps                    = BDSGlobalConstants::Instance()->GetCheckOverlaps();

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

      // prepare the placement name - if it's a duplicate placement, suffix the number of placement
      // to the base name. Increment the number afterwards.
      G4String placementName;
      if (thecurrentitem->GetNTimesPlaced() < 1)
	{placementName = name;}
      else
	{
	  std::stringstream namestream;
	  namestream << name << "_" << thecurrentitem->GetNTimesPlaced();
	  placementName = namestream.str();
	}
      thecurrentitem->IncrementNTimesPlaced();
      
      G4PVPlacement* elementPV = new G4PVPlacement(r,                     // its rotation
						   p,                     // its position
						   placementName + "_pv", // its name
						   elementLV,             // its logical volume
						   physiWorld,            // its mother  volume
						   false,	          // no boolean operation
						   nCopy,                 // copy number
						   checkOverlaps);        //overlap checking

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
	  readOutPV = new G4PVPlacement(rr,                       // its rotation
					rp,                       // its position
					placementName + "_ro_pv", // its name
					readOutLV,                // its logical volume
					readOutWorldPV,           // its mother  volume
					false,	                  // no boolean operation
					nCopy,                    // copy number
					checkOverlaps);           //overlap checking

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
      
      /*
      //this vector of physical volumes isn't used anywhere...
      fPhysicalVolumeVector.push_back(PhysiComponentPlace);
      std::vector<G4VPhysicalVolume*> MultiplePhysicalVolumes = thecurrentitem->GetMultiplePhysicalVolumes();
      for (unsigned int i=0;i<MultiplePhysicalVolumes.size(); i++)
	{fPhysicalVolumeVector.push_back(MultiplePhysicalVolumes.at(i));}
      */
      //this does nothing by default - only used by BDSElement
      //looks like it could just be done in its construction rather than
      //in BDSDetectorConstruction
      thecurrentitem->PrepareField(elementPV);
    }
  // set precision back
  G4cout.precision(G4precision);
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
