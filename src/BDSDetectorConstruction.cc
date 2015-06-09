//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.5-dev
//  
//
//
//   Geometry construction
//
//
//   History
//     19 May 2008 by Marchioni v.0.5-dev
//     18 Mar 2008 by Malton v.0.5-dev
//      3 Oct 2007 by Malton v.0.4
//     21 Nov 2006 by Agapov v.0.3
//     28 Mar 2006 by Agapov v.0.2
//     15 Dec 2005 by Agapov beta
//

#include <list>
#include <map>
#include <vector>

#include "BDSDetectorConstruction.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSSDManager.hh"

#include "G4UserLimits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "globals.hh"
#include "G4ios.hh"

#include "G4Navigator.hh"
#include "G4UniformMagField.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Material.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSLine.hh"
#include "BDSMaterials.hh"
#include "BDSTeleporter.hh"
#include "BDSLogicalVolumeInfo.hh"
#include "BDSComponentFactory.hh"

#include "G4MagneticField.hh"
#include "G4VSampler.hh"
#include "G4GeometrySampler.hh"

#include "ggmad.hh"
#include "parser/element.h"
#include "parser/elementlist.h"
#include "parser/enums.h"

#ifdef BDSDEBUG
bool debug = true;
#else
bool debug = false;
#endif

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
  
  if (verbose || debug) G4cout << "-->starting BDS construction \n"<<G4endl;
  //construct bds
  return ConstructBDS(beamline_list);
}

G4VPhysicalVolume* BDSDetectorConstruction::ConstructBDS(ElementList& beamline_list)
{  
  // prepare materials for this run
  BDSMaterials::Instance()->PrepareRequiredMaterials();
  
  // set global magnetic field first
  SetMagField(0.0); // necessary to set a global field; so choose zero
  
  // construct the component list
  BuildBeamline();

  // build world and calculate coordinates
  BuildWorld();

  // set default output formats for BDSDetector:
  int G4precision = G4cout.precision(15);
  
  // placement procedure
  ComponentPlacement();
  
  // construct tunnel
  BuildTunnel();

  // free the parser list
  std::list<struct Element>::iterator it;
  for(it = beamline_list.begin();it!=beamline_list.end();it++) {
    delete (*it).lst;
  }
  beamline_list.clear();
  
  if(verbose || debug) G4cout << __METHOD_NAME__ << "detector Construction done"<<G4endl; 

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
#endif

  if(verbose || debug) G4cout<<"Finished listing materials, returning physiWorld"<<G4endl; 
  
  // set precision back
  G4cout.precision(G4precision);

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
      
      BDSAcceleratorComponent* temp = theComponentFactory->createComponent(it, beamline_list);
      if(temp)
	{
	  if (temp->GetType() == "line")
	    {
	      // dynamic cast to access methods for iteration
	      BDSLine* line = dynamic_cast<BDSLine*>(temp);
	      if (line)
		{
		  //line of components to be added individually
		  for (BDSLine::BDSLineIterator i = line->begin(); i != line->end(); ++i)
		    {beamline->AddComponent(*i);}
		}
	    }
	  else
	    {
	      //single component
	      beamline->AddComponent(temp);
	    }
	}
    }

  // Special circular machine bits
  // Add teleporter to account for slight ring offset
  // Add terminator to do ring turn counting logic
  if (BDSExecOptions::Instance()->GetCircular())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      BDS::CalculateAndSetTeleporterDelta(beamline);
      BDSAcceleratorComponent* teleporter = theComponentFactory->createTeleporter();
      if (teleporter)
	{beamline->AddComponent(teleporter);}
      BDSAcceleratorComponent* terminator = theComponentFactory->createTerminator();
      if (terminator)
	{beamline->AddComponent(terminator);}
    }
  
  delete theComponentFactory;
      
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of the parser beamline element list: "<< beamline_list.size() << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "size of the constructed beamline: "<< beamline->size() << G4endl;
  
  if (beamline->size() == 0)
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
	{
	  G4cerr << __METHOD_NAME__ << "beamline element does not contain valid BDSAcceleratorComponent" << G4endl;
	  exit(1);
	}
      // check we can get the container logical volume to be placed
      G4LogicalVolume* elementLV = (*it)->GetContainerLogicalVolume();
      if (!elementLV)
	{
	  G4cerr << __METHOD_NAME__ << "this accelerator component " << (*it)->GetName() << " has no volume to be placed!" << G4endl;
	  exit(1);
	}
      
      G4String name = (*it)->GetName(); // this is done after the checks as it really just passes down to acc component
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placement of component named: " << name << G4endl;
#endif
      // read out geometry logical volume - note may not exist for each item - must be tested
      G4LogicalVolume* readOutLV   = thecurrentitem->GetReadOutLogicalVolume();
      // make read out geometry sensitive
      if (readOutLV)
	{readOutLV->SetSensitiveDetector(energyCounterSDRO);}
      
      // add the volume to one of the regions
      if(thecurrentitem->GetPrecisionRegion())
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "element is in the precision region" << G4endl;
#endif
	  elementLV->SetRegion(precisionRegion);
	  precisionRegion->AddRootLogicalVolume(elementLV);
	}
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "setting up sensitive volumes with read out geometry" << G4endl;
#endif
      // now register the spos and other info of this sensitive volume in global map
      // used by energy counter sd to get spos of that logical volume at histogram time
      BDSLogicalVolumeInfo* theinfo = new BDSLogicalVolumeInfo(name,
							       (*it)->GetSPositionMiddle());
      if(readOutLV)
	{BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(readOutLV,theinfo);}
      else
        {BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(elementLV,theinfo);}

      // this bit would also be unnecessary once all switched over to read out geometry
      // Register all logical volumes with sposition and any other information for later use
      std::vector<G4LogicalVolume*> allLVs = thecurrentitem->GetAllLogicalVolumes();
      std::vector<G4LogicalVolume*>::iterator allLVsIterator = allLVs.begin();
      for(;allLVsIterator != allLVs.end(); ++allLVsIterator)
	{
	  BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(*allLVsIterator,
							       new BDSLogicalVolumeInfo((*allLVsIterator)->GetName(),
											thecurrentitem->GetSPos())
							       );
	}

      // Use old way of setting sensitivity for volumes without read out LV
      // old way of setting sensitive volumes - remains for now for components that haven't been changed
      // in future will be done in all component constructors
      // NOTE this also sets GFLASH so most volumes won't have GFLASH now
      if (!readOutLV)
	{
	  std::vector<G4LogicalVolume*> SensVols = thecurrentitem->GetAllSensitiveVolumes();
	  std::vector<G4LogicalVolume*>::iterator sensIt= SensVols.begin();
	  for(;sensIt != SensVols.end(); ++sensIt)
	    {
	      //use already defined instance of Ecounter sd
	      (*sensIt)->SetSensitiveDetector(energyCounterSDRO);
	      //register any volume that an ECounter is attached to
	      BDSLogicalVolumeInfo* theinfo = new BDSLogicalVolumeInfo( (*sensIt)->GetName(),
									thecurrentitem->GetSPos() );
	      BDSGlobalConstants::Instance()->AddLogicalVolumeInfo((*sensIt),theinfo);
	      //set gflash parameterisation on volume if required
	      G4bool gflash     = BDSExecOptions::Instance()->GetGFlash();
	      if(gflash && ((*sensIt)->GetRegion() != precisionRegion) && (thecurrentitem->GetType()=="element"))
		{SetGFlashOnVolume(*sensIt);}
	    }
	}

      // get the placement details from the beamline component
      G4int nCopy         = thecurrentitem->GetCopyNumber();
      G4RotationMatrix* r = (*it)->GetRotationMiddle();
      G4ThreeVector     p = (*it)->GetPositionMiddle();
      
      G4PVPlacement* PhysiComponentPlace = new G4PVPlacement(r,                // its rotation
							     p,                // its position
							     name + "_pv",     // its name
							     elementLV,        // its logical volume
							     physiWorld,       // its mother  volume
							     false,	       // no boolean operation
							     nCopy,            // copy number
							     checkOverlaps);   //overlap checking

      // place read out volume in read out world - if this component has one
      if(readOutLV)
	{
	  // don't need the pointer for anything - purely instantiating registers it with g4
	  new G4PVPlacement(r,               // its rotation
			    p,               // its position
			    name + "_ro_ph", // its name
			    readOutLV,       // its logical volume
			    readOutWorldPV,  // its mother  volume
			    false,	     // no boolean operation
			    nCopy,           // copy number
			    checkOverlaps);  //overlap checking
	}


      //this vector of physical volumes isn't used anywhere...
      fPhysicalVolumeVector.push_back(PhysiComponentPlace);
      std::vector<G4VPhysicalVolume*> MultiplePhysicalVolumes = thecurrentitem->GetMultiplePhysicalVolumes();
      for (unsigned int i=0;i<MultiplePhysicalVolumes.size(); i++)
	{fPhysicalVolumeVector.push_back(MultiplePhysicalVolumes.at(i));}

      //this does nothing by default - only used by BDSElement
      //looks like it could just be done in its construction rather than
      //in BDSDetectorConstruction
      thecurrentitem->PrepareField(PhysiComponentPlace);
    }
}

void BDSDetectorConstruction::BuildTunnel()
{
  std::list<struct Element>::iterator it;
  for(it = beamline_list.begin();it!=beamline_list.end();it++)
    {
      if((*it).type==_TUNNEL ) {
#ifdef BDSDEBUG
	G4cout<<"BUILDING TUNNEL : "<<(*it).l<<"  "<<(*it).name<<G4endl;
#endif
	
	G4String gFormat="",  GFile="";
	G4String geometry = (*it).geometryFile;

	// get geometry format and file
	G4int pos = geometry.find(":");
	
	if(pos<0) { 
	  G4cerr<<"WARNING: invalid geometry reference format : "<<geometry<<G4endl;
	  gFormat="none";
	}
	
	else {
	  gFormat = geometry.substr(0,pos);
	  GFile = geometry.substr(pos+1,geometry.length() - pos); 
	}
	
#ifdef BDSDEBUG
	G4cout<<"placing components\n: geometry format - "<<gFormat<<G4endl<<
	  "file - "<<GFile<<G4endl;
#endif
	
	if(gFormat=="gmad") {
	 
	  GGmadDriver ggmad(GFile);
	  ggmad.Construct(logicWorld);
	  
	} else  G4cerr<<"Tunnel won't be build! "<<G4endl;
      }
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
