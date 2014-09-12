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

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

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
#include "G4SDManager.hh"
#include "G4RunManager.hh"

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
#include "BDSEnergyCounterSD.hh"
#include "BDSTeleporter.hh"
#include "BDSTerminator.hh"
#include "BDSLogicalVolumeInfo.hh"

#include "BDSComponentFactory.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"

#include "G4MagneticField.hh"

// GMAD interface
#include "ggmad.hh"

#include "G4VSampler.hh"
#include "G4GeometrySampler.hh"

#include "parser/element.h"
#include "parser/elementlist.h"
#include "parser/enums.h"
#include "parser/gmad.h"

//====================================

typedef std::list<BDSEnergyCounterSD*>  ECList;
ECList* theECList;

//--------------------------
// SYNCHROTRON RAD ***
G4double BDSLocalRadiusOfCurvature=DBL_MAX;// Used in Mean Free Path calc.
//--------------------------

typedef std::map<G4String,int> LogVolCountMap;
LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
LogVolMap* LogVol;

//=========================================

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
  _globalRotation(NULL)
{  
  verbose    = BDSExecOptions::Instance()->GetVerbose();
  gflash     = BDSExecOptions::Instance()->GetGFlash();
  gflashemax = BDSExecOptions::Instance()->GetGFlashEMax();
  gflashemin = BDSExecOptions::Instance()->GetGFlashEMin();
  
  //initialize world size vector
  itsWorldSize.push_back(0);
  itsWorldSize.push_back(1);
  itsWorldSize.push_back(2);

  //initialize global rotation matrix
  _globalRotation = new G4RotationMatrix();

  // GlashStuff                                                                                                                                                         
  theParticleBounds  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  theParticleBounds->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),gflashemax*CLHEP::GeV);
  theParticleBounds->SetMinEneToParametrise(*G4Electron::ElectronDefinition(),gflashemin*CLHEP::GeV);
  theParticleBounds->SetEneToKill(*G4Electron::ElectronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  
  theParticleBounds->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),gflashemax*CLHEP::GeV);
  theParticleBounds->SetMinEneToParametrise(*G4Positron::PositronDefinition(),gflashemin*CLHEP::GeV);
  theParticleBounds->SetEneToKill(*G4Positron::PositronDefinition(),BDSGlobalConstants::Instance()->GetThresholdCutCharged());

  theParticleBoundsVac  = new GFlashParticleBounds();              // Energy Cuts to kill particles                                                                
  theParticleBoundsVac->SetMaxEneToParametrise(*G4Electron::ElectronDefinition(),0*CLHEP::GeV);
  theParticleBoundsVac->SetMaxEneToParametrise(*G4Positron::PositronDefinition(),0*CLHEP::GeV);

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

  theHitMaker          = new GFlashHitMaker();                    // Makes the EnergieSpots 

}


//=================================================================

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  theECList   = new ECList;
  LogVolCount = new LogVolCountMap();
  LogVol      = new LogVolMap();
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
  std::list<struct Element>::iterator it;

  // prepare materials for this run
  PrepareRequiredMaterials();
  
  // set global magnetic field first
  SetMagField(0.0); // necessary to set a global field; so chose zero
  
  // Special ring machine bits
  // Add teleporter to account for slight ring offset
  // Add terminator to do ring turn counting logic
  // Both only in case of a circular machine
  // must be done before we parse the element list (for correct coordinates)
  if (BDSExecOptions::Instance()->GetCircular()) {
#ifdef BDSDEBUG
    G4cout << "Circular machine - this is the last element - creating terminator & teleporter" << G4endl;
#endif
    AddTeleporterToEndOfBeamline(&beamline_list);
    AddTerminatorToEndOfBeamline(&beamline_list);
  }


  // convert the parsed element list to list of BDS elements
  //
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();

  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  for(it = beamline_list.begin();it!=beamline_list.end();it++){

#ifdef BDSDEBUG
    G4cout << "BDSDetectorConstruction creating component " << (*it).name << G4endl;
#endif

    BDSAcceleratorComponent* temp = theComponentFactory->createComponent(it, beamline_list);

#ifdef BDSDEBUG
    G4cout << "pushing onto back of beamline..." << G4endl;
#endif
    if(temp){
      BDSBeamline::Instance()->addComponent(temp);
      //For the outline file...
      BDSBeamline::Instance()->lastItem()->SetK1((*it).k1);
      BDSBeamline::Instance()->lastItem()->SetK2((*it).k2);
      BDSBeamline::Instance()->lastItem()->SetK3((*it).k3);
    }

#ifdef BDSDEBUG
    G4cout << "done." << G4endl;
#endif
  }
  
  delete theComponentFactory;
  theComponentFactory = NULL;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of beamline element list: "<< beamline_list.size() << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "size of theBeamline: "<< BDSBeamline::Instance()->size() << G4endl;
  
  // construct the component list
  //

  if (verbose || debug) G4cout << "now constructing geometry" << G4endl;
  
  std::list<BDSAcceleratorComponent*>::const_iterator iBeam;
  
  G4ThreeVector rtot = G4ThreeVector(0.,0.,0.);   // world dimension
  G4ThreeVector rlast = G4ThreeVector(0.,0.,0.);  // edge of last element coordinates
  G4ThreeVector rextentmax = G4ThreeVector(0.,0.,0.);  // extent
  G4ThreeVector rextentmin = G4ThreeVector(0.,0.,0.);  // extent
  G4ThreeVector rmin = G4ThreeVector(0.,0.,0.);
  G4ThreeVector rmax = G4ThreeVector(0.,0.,0.);

  G4ThreeVector localX = G4ThreeVector(1,0,0);  // local coordinate axis
  G4ThreeVector localY = G4ThreeVector(0,1,0);
  G4ThreeVector localZ = G4ThreeVector(0,0,1);

  G4double s_tot = 0; // position along the beamline

  // define geometry scope - to calculate world dimensions
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    {
      BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();

#ifdef BDSDEBUG 
      G4cout << thecurrentitem->GetName() << "  "
             << thecurrentitem->GetLength() << "  "
             << thecurrentitem->GetAngle() << "  "
             << G4endl;
#endif

      thecurrentitem->SetSPos(s_tot+thecurrentitem->GetArcLength()/2.0);

      // advance coordinates , but not for cylindrical sampler
      if(( thecurrentitem->GetType() != "csampler") || ( thecurrentitem->GetLength() <= BDSGlobalConstants::Instance()->GetSamplerLength() ) )
	{
	  s_tot+= thecurrentitem->GetArcLength();

	  G4double angle=thecurrentitem->GetAngle();
	  if(!angle && thecurrentitem->GetType()=="transform3d")
	    angle=thecurrentitem->GetPhi();
	  G4double theta=thecurrentitem->GetTheta();
	  G4double psi=thecurrentitem->GetPsi();

	  // define new coordinate system local frame	  
	  localX.rotate(psi,localZ);
	  localY.rotate(psi,localZ);
	  localZ.rotate(psi,localZ);

	  localX.rotate(angle,localY);
	  localY.rotate(angle,localY);
	  localZ.rotate(angle,localY);

	  localX.rotate(theta,localX);
	  localY.rotate(theta,localX);
	  localZ.rotate(theta,localX);

	  // advance the coordinate system translation
	  rtot += localZ * thecurrentitem->GetZLength();
#ifdef BDSDEBUG
          G4cout << thecurrentitem->GetType() << " " << rtot << G4endl;
#endif
	}


      rextentmax = rtot + localX*thecurrentitem->GetXLength() + localY*thecurrentitem->GetYLength();
      rextentmin = rtot - localX*thecurrentitem->GetXLength() - localY*thecurrentitem->GetYLength();

      //      rextentmax(0) = rtot(0) + (localX*thecurrentitem->GetXLength())(0) + (localX*thecurrentitem->GetYLength())(0);
      //      rextentmax(1) = rtot(1) + (localY*thecurrentitem->GetXLength())(1) + (localY*thecurrentitem->GetYLength())(1);
      //      rextentmax(2) = rtot(2) + (localZ*thecurrentitem->GetXLength())(2) + (localZ*thecurrentitem->GetYLength())(2);

      //      rextentmin(0) = rtot(0) - localX*thecurrentitem->GetXLength() - localX*thecurrentitem->GetYLength();
      //      rextentmin(1) = rtot(1) - localY*thecurrentitem->GetXLength() - localY*thecurrentitem->GetYLength();
      //      rextentmin(2) = rtot(2) - localZ*thecurrentitem->GetXLength() - localZ*thecurrentitem->GetYLength();


	for(int i=0; i<3; i++){
	  rmax(i) = std::max(rextentmax(i),rmax(i));
	  rmin(i) = std::min(rextentmin(i),rmin(i));
	}
    }

  // determine the world size  
  G4String LocalName="World";
  
  SetWorldSizeX(1.5*( 2*std::max(fabs( rmin(0) ),fabs( rmax(0) ) ) ) *CLHEP::mm);
  SetWorldSizeY(1.5*( 2*std::max(fabs(rmin(1)),fabs(rmax(1))) ) *CLHEP::mm);
  SetWorldSizeZ(1.5*(fabs(rmin(2)) + fabs(rmax(2))) *CLHEP::mm);

  if(verbose || debug)
    {
      
      G4cout<<"minX="<<rmin(0)/CLHEP::m<<" m"<<" maxX="<<rmax(0)/CLHEP::m<<" m"<<G4endl;
      G4cout<<"minY="<<rmin(1)/CLHEP::m<<" m"<<" maxY="<<rmax(1)/CLHEP::m<<" m"<<G4endl;
      G4cout<<"minZ="<<rmin(2)/CLHEP::m<<" m"<<" maxZ="<<rmax(2)/CLHEP::m<<" m"<<G4endl;

      G4cout<<"itsWorldSizeX = "<<GetWorldSizeX()/CLHEP::m<<G4endl;
      G4cout<<"itsWorldSizeY = "<<GetWorldSizeY()/CLHEP::m<<G4endl;
      G4cout<<"itsWorldSizeZ = "<<GetWorldSizeZ()/CLHEP::m<<G4endl;
      
      G4cout<<"box size="<<BDSGlobalConstants::Instance()->GetComponentBoxSize()/CLHEP::m<<" m"<<G4endl;
      G4cout<<"s_tot="<<s_tot/CLHEP::m<<" m"<<G4endl;
    }

  BDSGlobalConstants::Instance()->SetSMax(s_tot);

  solidWorld = new G4Box("World", GetWorldSizeX(), GetWorldSizeY(), GetWorldSizeZ());
    
  logicWorld = new G4LogicalVolume(solidWorld,	       //its solid
				   BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()), //its material
				   "World");	       //its name
  
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);
  // set world volume visibility for debugging
#ifdef BDSDEBUG 
  logicWorld->SetVisAttributes(new G4VisAttributes(true));	
#endif
	
  // set default max step length (only for particles which have the
  // G4StepLimiter process enabled)
#ifndef NOUSERLIMITS
  G4UserLimits* WorldUserLimits =new G4UserLimits();
  WorldUserLimits->SetMaxAllowedStep(GetWorldSizeZ());
  WorldUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  WorldUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  logicWorld->SetUserLimits(WorldUserLimits);
#endif


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
  precisionRegion->SetUserLimits(WorldUserLimits);
#endif

  // world

  physiWorld = new G4PVPlacement((G4RotationMatrix*)0,		// no rotation
  				 (G4ThreeVector)0,             // at (0,0,0)
                                 logicWorld,	// its logical volume
                                 LocalName,	// its name
                                 NULL,		// its mother  volume
                                 false,		// no boolean operation
                                 0,             // copy number
				 BDSGlobalConstants::Instance()->GetCheckOverlaps());		// overlap checking



  // sensitive detectors

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  G4ThreeVector TargetPos;

  // set default output formats for BDSDetector:
  int G4precision = G4cout.precision(15);
  
#ifdef BDSDEBUG 
  G4cout<<"total length="<<s_tot/CLHEP::m<<"m"<<G4endl;
#endif
  
  // reset counters:
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next()){
    BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();

    // zero length components have no logical volumes
    if(thecurrentitem->GetLength()!=0.)
      {
	G4String logVolName = thecurrentitem->GetMarkerLogicalVolume()->GetName();
	(*LogVolCount)[logVolName]=1;
      }
  }
  
  if (verbose || debug) G4cout<<"starting placement procedure "<<G4endl;
  
  rtot = G4ThreeVector(0.,0.,0.);
  localX = G4ThreeVector(1.,0.,0.); 
  localY = G4ThreeVector(0.,1.,0.);
  localZ = G4ThreeVector(0.,0.,1.);
  
  //you only need a single instance of your sensitive detector class
  //attach to as many logical volumes as you want
  //note each new sensitive detector invokes a slow string compare
  //while registering with sd manager.  ok if only a few SD types.
  BDSEnergyCounterSD* ECounter    = new BDSEnergyCounterSD("base_ec");
  //BDSTerminatorSD*    TurnCounter = new BDSTerminatorSD("ring_counter");
  SDman->AddNewDetector(ECounter);
  //SDman->AddNewDetector(TurnCounter);
  theECList->push_back(ECounter);
  
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    {
      BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();
      //thecurrentitem->SetZLower(rtot.z());
#ifdef BDSDEBUG
      G4cout << G4endl;
#endif
      G4double angle=thecurrentitem->GetAngle();
      G4double theta=thecurrentitem->GetTheta();
      G4double psi = thecurrentitem->GetPsi();
      G4double tilt = thecurrentitem->GetTilt();
      G4double phi = thecurrentitem->GetPhi();
      G4double length = thecurrentitem->GetZLength();

      if( thecurrentitem->GetType() == "transform3d")
	{

#ifdef BDSDEBUG 
          G4cout<<"transform3d : "<<phi<<" "<<theta<<" "<<psi<<G4endl;
#endif


	  rtot(0) += thecurrentitem->GetXOffset(); 
	  rtot(1) += thecurrentitem->GetYOffset(); 
	  rtot(2) += thecurrentitem->GetZOffset(); 

	  rlast(0) += thecurrentitem->GetXOffset();
	  rlast(1) += thecurrentitem->GetYOffset(); 
	  rlast(2) += thecurrentitem->GetZOffset(); 

	  _globalRotation->rotate(psi,localZ);
	  localX.rotate(psi,localZ);
	  localY.rotate(psi,localZ);

	  _globalRotation->rotate(phi,localY);
	  localX.rotate(phi,localY);
	  localZ.rotate(phi,localY);
	  
	  _globalRotation->rotate(theta,localX);
	  localY.rotate(theta,localX);
	  localZ.rotate(theta,localX);
	  	  	  
	  continue;
	}
      
      // rotation matrix for component placement
      G4RotationMatrix *rotateComponent = new G4RotationMatrix;

      // tilted bends influence reference frame, otherwise just local tilt
      if(thecurrentitem->GetType() == "sbend" || thecurrentitem->GetType() == "rbend" )
	{
	  _globalRotation->rotate(tilt,localZ);
	  localX.rotate(tilt,localZ);
	  localY.rotate(tilt,localZ);
	}
      else 
	rotateComponent->rotateZ(tilt);
    
      // define center of bended elements from the previous coordinate frame
      G4ThreeVector zHalfAngle = localZ; 

      if( thecurrentitem->GetType() == "sbend" || thecurrentitem->GetType() == "rbend"  )
	zHalfAngle.rotate(angle/2,localY);

#ifdef BDSDEBUG
      G4cout<<"zHalfAngle="<<zHalfAngle<<G4endl;
      G4cout<<"localZ="<<localZ<<G4endl;
      G4cout<<"localX="<<localX<<G4endl;
      G4cout<<"localY="<<localY<<G4endl;
      G4cout<<"rlast="<<rlast<<G4endl;
#endif
      
      // target position
      TargetPos = rlast + zHalfAngle *  ( length/2 + BDSGlobalConstants::Instance()->GetLengthSafety()/2 ) ;

#ifdef BDSDEBUG 
      G4cout<<"TargetPos="<<TargetPos<<G4endl;
#endif

      // advance the coordinates, but not for cylindrical samplers 
      if( ( ( thecurrentitem->GetType() != "csampler") || ( length <= BDSGlobalConstants::Instance()->GetSamplerLength() ) )  && ( thecurrentitem->GetType()!="element" ))
	{
#ifdef BDSDEBUG 
          G4cout << thecurrentitem->GetType() << " "
                 << thecurrentitem->GetName() << " "
                 << G4endl;
#endif
	  rtot = rlast + zHalfAngle * ( length/2 + BDSGlobalConstants::Instance()->GetLengthSafety()/2 );
	  rlast = rtot + zHalfAngle * ( length/2 + BDSGlobalConstants::Instance()->GetLengthSafety()/2 );
	}

      // rotate to the previous reference frame
      rotateComponent->transform(*_globalRotation);

      rotateComponent->invert();

      // recompute global rotation
      // define new coordinate system local frame	  
 
      // bends transform the coordinate system
      if( thecurrentitem->GetType() == "sbend" || thecurrentitem->GetType() == "rbend")
	{
	  _globalRotation->rotate(angle,localY);
	  localX.rotate(angle,localY);
	  localZ.rotate(angle,localY);
	  
	  
	  _globalRotation->rotate(theta,localX);
	  localY.rotate(theta,localX);
	  localZ.rotate(theta,localX);
	  
	  // bend trapezoids defined along z-axis
	  rotateComponent->rotateY(-CLHEP::twopi/4-angle/2); 						
	} else {
	if (thecurrentitem->GetMarkerLogicalVolume()->GetSolid()->GetName().contains("trapezoid") ){
	  rotateComponent->rotateY(-CLHEP::twopi/4); //Drift trapezoids defined along z axis 
	}
      }
    
      // zero length components not placed (transform3d)
      if(length!=0.){
	G4LogicalVolume* LocalLogVol=thecurrentitem->GetMarkerLogicalVolume();
	
	G4String LogVolName=LocalLogVol->GetName();
	// Set visualisation options for marker volumes - perhaps should be in base class..
	G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.4));
	VisAtt1->SetForceSolid(true);  
	// Set visible only if debug build, otherwise hidden
#if defined BDSDEBUG
	VisAtt1->SetVisibility(true);
#else
	VisAtt1->SetVisibility(false);
#endif
	LocalLogVol->SetVisAttributes(VisAtt1);
	//------------
	int nCopy=(*LogVolCount)[LogVolName]-1;
	(*LogVolCount)[LogVolName]++;
	
	/*
	// now register the spos and other info of this sensitive volume in global map
	// used by energy counter sd to get spos of that logical volume at histogram time
	BDSLogicalVolumeInfo* theinfo = new BDSLogicalVolumeInfo( thecurrentitem->GetName(),
								  thecurrentitem->GetSPos() );
	BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(SensVol,theinfo);
	*/
	// now register the spos and other info of this sensitive volume in global map
	    // used by energy counter sd to get spos of that logical volume at histogram time
	BDSLogicalVolumeInfo* theinfo = new BDSLogicalVolumeInfo( LogVolName,
								  thecurrentitem->GetSPos() );
	BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(LocalLogVol,theinfo);

	// add the volume to one of the regions
	if(thecurrentitem->GetPrecisionRegion())
	  {
#ifdef BDSDEBUG
	    G4cout<<"ELEMENT IS IN PRECISION REGION: "<<thecurrentitem->GetPrecisionRegion()<< G4endl;
#endif
	    LocalLogVol->SetRegion(precisionRegion);
	    precisionRegion->AddRootLogicalVolume(LocalLogVol);
	  }

	
#ifdef BDSDEBUG
	G4cout<<"SETTING UP SENSITIVE VOLUMES..."<< G4endl;
#endif	
	// set up the sensitive volumes for energy counting:
	thecurrentitem->SetCopyNumber(nCopy);

	std::vector<G4LogicalVolume*> SensVols = thecurrentitem->GetSensitiveVolumes();
	if( ( thecurrentitem->GetType()!="sampler" && thecurrentitem->GetType()!="csampler" )
	    && SensVols.size()>0)
	  {
	    thecurrentitem->SetBDSEnergyCounter(ECounter);
	    for(G4int i=0; i<(G4int)SensVols.size(); i++)
	      {
		//use already defined instance of Ecounter sd
		SensVols[i]->SetSensitiveDetector(ECounter);	
		//register any volume that an ECounter is attached to
		BDSLogicalVolumeInfo* theinfo = new BDSLogicalVolumeInfo( SensVols[i]->GetName(),
									  thecurrentitem->GetSPos() );
		BDSGlobalConstants::Instance()->AddLogicalVolumeInfo(SensVols[i],theinfo);
		//G4cout << "sensvols["<<i<<"] - name : "<<SensVols[i]->GetName() << G4endl;
		
		if(gflash){
		  if((SensVols[i]->GetRegion() != precisionRegion) && (thecurrentitem->GetType()=="element")){//If not in the precision region....
		    //		    if(SensVols[i]->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
#ifdef BDSDEBUG
		    G4cout << "...adding " << SensVols[i]->GetName() << " to gFlashRegion" << G4endl;
#endif
		    /**********************************************                                                                                                                       
		     * Initialise shower model                                                                                                                                          
		     ***********************************************/
		    G4String rname = "gFlashRegion_" + SensVols[i]->GetName();
		    gFlashRegion.push_back(new G4Region(rname.c_str()));
		    G4String mname = "fastShowerModel" + rname;
#ifdef BDSDEBUG
		    G4cout << "...making parameterisation..." << G4endl;
#endif
		    theFastShowerModel.push_back(new BDSShowerModel(mname.c_str(),gFlashRegion.back()));
		    theParameterisation.push_back(new GFlashHomoShowerParameterisation(BDSMaterials::Instance()->GetMaterial(SensVols[i]->GetMaterial()->GetName().c_str()))); 
		    theFastShowerModel.back()->SetParameterisation(*theParameterisation.back());
		    theFastShowerModel.back()->SetParticleBounds(*theParticleBounds) ;
		    theFastShowerModel.back()->SetHitMaker(*theHitMaker);
		    if(SensVols[i]->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
		      theFastShowerModel.back()->SetFlagParamType(1);//Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
		      theFastShowerModel.back()->SetFlagParticleContainment(1);//Turn on containment
		    } else {
		      theFastShowerModel.back()->SetFlagParamType(0);//Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
		      theFastShowerModel.back()->SetFlagParticleContainment(0);//Turn on containment

		    }
		    SensVols[i]->SetRegion(gFlashRegion.back());
		    gFlashRegion.back()->AddRootLogicalVolume(SensVols[i]);
		    //		    gFlashRegion.back()->SetUserLimits(new G4UserLimits(thecurrentitem->GetLength()/10.0));
		    //		    SensVols[i]->SetUserLimits(new G4UserLimits(thecurrentitem->GetLength()/10.0));
		  }		  
		}
	      }

	  }


	    //Loop through again, unsetting gas regions

	//	if(SensVols.size()>0){
	//	  for(G4int i=0; i<(G4int)SensVols.size(); i++){
	//	    if(SensVols[i]->GetMaterial()->GetState()==kStateGas){ //If the region material state is not gas, associate with a parameterisation
	//	      SensVols[i]->GetRegion()->RemoveRootLogicalVolume(SensVols[i]);
	      //	      SensVols[i]->SetRegion(NULL);

	      //	      SensVols[i]->SetRegion(gasRegion);
	      //	      SensVols[i]->SetUserLimits(new G4UserLimits(thecurrentitem->GetLength()/10.0));
	//      }
	//      }
	//      }

	
	// count and store sampler names. Should go into constructors!
  
	LocalName=thecurrentitem->GetName()+"_phys";
	if(thecurrentitem->GetType()=="sampler") {
	  BDSSampler::outputNames.push_back(LocalName + "_" + BDSGlobalConstants::Instance()->StringFromInt(nCopy+1));
	} 
	else if(thecurrentitem->GetType()=="csampler") {
	  BDSSamplerCylinder::outputNames.push_back(LocalName + "_" + BDSGlobalConstants::Instance()->StringFromInt(nCopy+1));
	} else {
	  //it would be nice to set correctly names also for other elements...
	  //but need to count them!
	}

	/*
	//for torus sbend
	if(thecurrentitem->GetType() == "sbend") {

	  G4double rho = length/fabs(angle);

	  G4RotationMatrix* Rot=new G4RotationMatrix();
	  Rot->rotateX(-pi/2 * rad);
	  //Rot->rotateZ(pi * rad);
	  //Rot->rotateZ(- ( pi/2 - fabs(angle)/2 ) * rad);
	  TargetPos -= zHalfAngle *  ( length/2 + BDSGlobalConstants::Instance()->GetLengthSafety()/2 ) ;
	  TargetPos+=G4ThreeVector(-rho,0,0);
	  //TargetPos=G4ThreeVector(0,0,rho);
	  //if(angle < 0)
	  //{
	  //  Rot->rotateZ(pi);
	  //  TargetPos=G4ThreeVector(rho,0,0);
	  //}
	  rotateComponent=Rot;
	}
	*/

#ifdef BDSDEBUG
	G4cout<<"ALIGNING COMPONENT..."<< G4endl;
#endif	
	// Align Component - most cases does nothing. 
	// Currently only used for BDSElement	
	// For other elements stores global position and rotation,
	// needed for BDSOutline
	thecurrentitem->AlignComponent(//TargetPos,
				 rlast,
				 rotateComponent,
				 *_globalRotation,
				 rtot,
				 rlast,
				 localX,
				 localY,
				 localZ);

#ifdef BDSDEBUG
	G4cout<<"Placing PHYSICAL COMPONENT..."<< G4endl;
	G4cout << "BDSDetectorConstruction : rotateComponent = " << *rotateComponent << G4endl;
	G4cout << "BDSDetectorConstruction : TargetPos        = " << TargetPos << G4endl;
#endif	

	G4PVPlacement* PhysiComponentPlace = 
	  new G4PVPlacement(
			    rotateComponent,  // its rotation
			    TargetPos,        // its position
			    LocalName,	      // its name
			    LocalLogVol,      // its logical volume
			    physiWorld,	      // its mother  volume
			    false,	      // no boolean operation
			    nCopy,            // copy number
			    BDSGlobalConstants::Instance()->GetCheckOverlaps());	      //overlap checking

	  fPhysicalVolumeVector.push_back(PhysiComponentPlace);
	  std::vector<G4VPhysicalVolume*> MultiplePhysicalVolumes = thecurrentitem->GetMultiplePhysicalVolumes();
	  for (unsigned int i=0;i<MultiplePhysicalVolumes.size(); i++) fPhysicalVolumeVector.push_back(MultiplePhysicalVolumes.at(i));
					    
#ifdef BDSDEBUG 
        G4cout << "Volume name: " << LocalName << G4endl;
#endif
        if(BDSGlobalConstants::Instance()->GetRefVolume()+"_phys"==LocalName && 
           BDSGlobalConstants::Instance()->GetRefCopyNo()==nCopy){
#ifdef BDSDEBUG 
          G4cout << "Setting new transform" <<G4endl;
#endif
	  G4AffineTransform tf(*_globalRotation,TargetPos-G4ThreeVector(0,0,length/2));
	  BDSGlobalConstants::Instance()->SetRefTransform(tf);
        }

	thecurrentitem->PrepareField(PhysiComponentPlace);
      }
    }

#ifdef BDSDEBUG
  // check of logvolinfo
  // LN TEST
  //  typedef std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>::iterator it_type;
  std::map<G4LogicalVolume*,BDSLogicalVolumeInfo*>* themap = BDSGlobalConstants::Instance()->LogicalVolumeInfo();
  //for (it_type iterator = themap->begin(); iterator != themap->end(); iterator++){
    //G4cout << "pointer : " << iterator->first << "\tname : " << iterator->second->GetName() << "\t" 
    //	   << iterator->second->GetSPos()/CLHEP::m << G4endl;
  //}
  G4cout << themap->size() << G4endl;

  //LN TEST of spos
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    {
      BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();
      G4double currentspos = thecurrentitem->GetSPos();
      G4String currentname = thecurrentitem->GetName();
      G4cout << "name : " << currentname << "\t" 
	     << "spos : " << currentspos/CLHEP::m << " m" <<G4endl
	     << "length   : " << thecurrentitem->GetLength()/CLHEP::m << " m" << G4endl
	     << "xlength  : " << thecurrentitem->GetXLength()/CLHEP::m << " m" << G4endl
	     << "ylength  : " << thecurrentitem->GetYLength()/CLHEP::m << " m" << G4endl
	     << "zlength  : " << thecurrentitem->GetZLength()/CLHEP::m << " m" << G4endl
	     << G4endl;
    }
  
#endif
  // construct tunnel
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
  
  // free the parser list
  for(it = beamline_list.begin();it!=beamline_list.end();it++) {
    delete (*it).lst;
  }
  beamline_list.clear();

  if(verbose || debug) G4cout<<"end placement, size="<<BDSBeamline::Instance()->size()<<G4endl;
  
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
  LogVolCount->clear();
  delete LogVolCount;
  //LogVolCount = NULL;
  
  //LogVolMap::iterator iter;
  //for(iter=LogVol->begin();iter!=LogVol->end();iter++){
  //  delete (*iter).second;
  // }
  LogVol->clear();
  delete LogVol;
  //LogVol = NULL;

  delete theECList;
  //theECList = NULL;

  delete precisionRegion;
  gFlashRegion.clear();

  delete _globalRotation;

  delete theHitMaker;
  delete theParticleBounds;
  delete theParticleBoundsVac;
}

//=================================================================

G4double* BDSDetectorConstruction::GetWorldSize(){
  int s=3;
  G4double* ret = new G4double[s];
  for(int i=0; i<s; i++){
    ret[i]=itsWorldSize[i];
  }
  return ret;
}

G4double BDSDetectorConstruction::GetWorldSizeX(){
  return itsWorldSize[0];
}

G4double BDSDetectorConstruction::GetWorldSizeY(){
  return itsWorldSize[1];
}

G4double BDSDetectorConstruction::GetWorldSizeZ(){
  return itsWorldSize[2];
}

void BDSDetectorConstruction::SetWorldSizeX(G4double val){
  itsWorldSize[0]=val;
}

void BDSDetectorConstruction::SetWorldSizeY(G4double val){
  itsWorldSize[1]=val;
}

void BDSDetectorConstruction::SetWorldSizeZ(G4double val){
  itsWorldSize[2]=val;
}
