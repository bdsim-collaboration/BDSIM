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

//=================================================================

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
#include "G4GeometryManager.hh"
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
#include "BDSTerminator.hh"
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

//=================================================================

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


//=================================================================

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

#ifdef BDSDEBUG
  G4cout << BDSAcceleratorModel::Instance()->GetFlatBeamline();
#endif
  
  // construct tunnel
  BuildTunnel();

  // free the parser list
  std::list<struct Element>::iterator it;
  for(it = beamline_list.begin();it!=beamline_list.end();it++) {
    delete (*it).lst;
  }
  beamline_list.clear();
  
  if(verbose || debug) G4cout<<"Detector Construction done"<<G4endl; 

#ifdef BDSDEBUG 
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
#endif

  if(verbose || debug) G4cout<<"Finished listing materials, returning physiWorld"<<G4endl; 
  
  // set precision back
  G4cout.precision(G4precision);

  return physiWorld;
}

//=================================================================
 
void BDSDetectorConstruction::SetMagField(const G4double fieldValue){
  
  G4FieldManager* fieldMgr =
    G4TransportationManager::GetTransportationManager()->GetFieldManager();
  magField = new G4UniformMagField(G4ThreeVector(0.,fieldValue,0.));  
  fieldMgr->SetDetectorField(magField);
  fieldMgr->CreateChordFinder(magField);
}

//=================================================================
BDSDetectorConstruction::~BDSDetectorConstruction()
{ 
  delete precisionRegion;
  gFlashRegion.clear();

  delete _globalRotation;

  delete theHitMaker;
  delete theParticleBounds;
  //  delete theParticleBoundsVac;
}

//=================================================================
void BDSDetectorConstruction::BuildBeamline(){
  std::list<struct Element>::iterator it;

  // convert the parsed element list to list of BDS elements
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
		    {
		      beamline->AddComponent(*i);
		      //BDSBeamline::Instance()->addComponent(*i);}
		    }
		  delete temp;
		}
	    }
	  else
	    {
	      //single component
	      //BDSBeamline::Instance()->addComponent(temp);
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
  G4cout << __METHOD_NAME__ << "size of beamline element list: "<< beamline_list.size() << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "size of the beamline: "<< beamline->size() << G4endl;
  
  if (beamline->size() == 0)
    {
      G4cout << __METHOD_NAME__ << "beamline empty or no line selected! exiting" << G4endl;
      exit(1);
    }
  // register the beamline in the holder class for the full model
  BDSAcceleratorModel::Instance()->RegisterFlatBeamline(beamline);
}

void BDSDetectorConstruction::BuildWorld(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  
  G4ThreeVector maxpositive = beamline->GetMaximumExtentPositive();
  G4ThreeVector maxnegative = beamline->GetMaximumExtentNegative();
  G4ThreeVector worldR;
  for (int i = 0; i < 3; i++){
    worldR[i] = std::max(fabs(maxpositive[i]),fabs(maxnegative[i]));
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " world extent positive : " << maxpositive << G4endl;
  G4cout << __METHOD_NAME__ << " world extent negative : " << maxnegative << G4endl;
  G4cout << __METHOD_NAME__ << " world half size determined to be strictly: " << worldR << G4endl;
#endif
  worldR += G4ThreeVector(5000,5000,5000); //add 5m extra in every dimension
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " with 5m margin becomes in all dimensions : " << worldR << G4endl;
#endif
  
  G4String worldName="World";
  solidWorld = new G4Box(worldName, worldR.x(), worldR.y(), worldR.z());

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  logicWorld = new G4LogicalVolume(solidWorld,	       // solid
				   emptyMaterial,      // material
				   worldName);	       // name

  // read out geometry logical volume
  // note g4logicalvolume has a private copy constructor so we have to repeat everything here annoyingly
  G4LogicalVolume* readOutWorldLV = new G4LogicalVolume(solidWorld,    // solid
							emptyMaterial, // material
							worldName);    // name
  
  // visual attributes
  if (BDSExecOptions::Instance()->GetVisDebug()) {
    G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
    debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
    logicWorld->SetVisAttributes(debugWorldVis);
    readOutWorldLV->SetVisAttributes(debugWorldVis);
  } else {
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

void BDSDetectorConstruction::ComponentPlacement(){
  if (verbose || debug) G4cout<<"starting placement procedure "<<G4endl;

  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  
  G4ThreeVector TargetPos;          // position of component
  G4ThreeVector rlast = G4ThreeVector(0.,0.,0.);  // edge of last element coordinates
  G4ThreeVector rtot(0.,0.,0.);     // position of component, often same as TargetPos
  G4ThreeVector localX(1.,0.,0.); 
  G4ThreeVector localY(0.,1.,0.);
  G4ThreeVector localZ(0.,0.,1.);

  // few general variables that we don't need to get every
  // time in the loop for component placement
  G4VPhysicalVolume* readOutWorldPV       = BDSAcceleratorModel::Instance()->GetReadOutWorldPV();
  G4VSensitiveDetector* energyCounterSDRO = BDSSDManager::Instance()->GetEnergyCounterOnAxisSDRO();
  G4bool checkOverlaps                    = BDSGlobalConstants::Instance()->GetCheckOverlaps();

  BDSBeamlineIterator it = beamline->begin();
  for(; it != beamline->end(); ++it)
    {
      BDSAcceleratorComponent* thecurrentitem = (*it)->GetAcceleratorComponent();
      
      // get the logical volume to be placed
      G4LogicalVolume* elementLV = (*it)->GetContainerLogicalVolume();
      G4cout << elementLV->GetName()<< G4endl;
      G4String         name      = (*it)->GetName();
#ifdef BDSDEBUG
      G4cout << "Placement of component named: " << name << G4endl;
      G4cout << thecurrentitem->GetName() << G4endl;
#endif
      // read out geometry logical volume - note may not exist for each item - must be tested
      G4LogicalVolume* readOutLV   = thecurrentitem->GetReadOutLogicalVolume();
      //G4LogicalVolume* readOutLV   = thecurrentitem->GetReadOutLogicalVolume();
      
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
      G4cout<<"SETTING UP SENSITIVE VOLUMES..."<< G4endl;
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

      // make read out geometry sensitive
      if (readOutLV)
	{readOutLV->SetSensitiveDetector(energyCounterSDRO);}
      
      // old way of setting sensitive volumes - remains for now for components that haven't been changed
      // in future will be done in all component constructors
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
					    
#ifdef BDSDEBUG 
      G4cout << "Volume name: " << name << G4endl;
#endif
      if(BDSGlobalConstants::Instance()->GetRefVolume()+"_phys"== name && 
	 BDSGlobalConstants::Instance()->GetRefCopyNo()==nCopy){
#ifdef BDSDEBUG 
	G4cout << "Setting new transform" <<G4endl;
#endif
	//G4AffineTransform tf(*_globalRotation,TargetPos-G4ThreeVector(0,0,length/2));
	//BDSGlobalConstants::Instance()->SetRefTransform(tf);
      }

      //this does nothing by default - only used by BDSElement
      //looks like it could just be done in its construction rather than
      //in BDSDetectorConstruction
      thecurrentitem->PrepareField(PhysiComponentPlace);
    }
}

void BDSDetectorConstruction::BuildTunnel(){
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
