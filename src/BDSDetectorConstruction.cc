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

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSDetectorConstruction.hh"

#include "G4UserLimits.hh"
#include "G4GeometryManager.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4Tubs.hh"
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
#include <iostream>
#include <list>
#include <map>
#include "BDSAcceleratorComponent.hh"

#include "G4Navigator.hh"
#include "G4UniformMagField.hh"

#include "G4Material.hh"
#include "BDSEnergyCounterSD.hh"

// elements
#include "BDSDrift.hh"
#include "BDSPCLDrift.hh"
#include "BDSSectorBend.hh"
#include "BDSQuadrupole.hh"
#include "BDSTMultipole.hh"
#include "BDSRfCavity.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSLaserWire.hh"
#include "BDSLWCalorimeter.hh"
#include "BDSElement.hh"
// output interface
#include "BDSOutput.hh"
#include "BDSComponentFactory.hh"

#include "G4MagneticField.hh"

// GMAD interface
#include "ggmad.hh"

#include "G4VSampler.hh"
#include "G4GeometrySampler.hh"
#include "G4IStore.hh"
 
using namespace std;

//====================================

//typedef list<BDSAcceleratorComponent*>  BDSBeamline;

typedef list<BDSEnergyCounterSD*>  ECList;
ECList* theECList;

//extern BDSGlobalConstants* BDSGlobalConstants::Instance();
extern G4int gflash;
extern G4double gflashemax;
extern G4double gflashemin;

//--------------------------
// SYNCHROTRON RAD ***
G4double BDSLocalRadiusOfCurvature=DBL_MAX;// Used in Mean Free Path calc.
//--------------------------

G4Material* aMaterial;
extern G4double NumSpoilerRadLen;

typedef std::map<G4String,int> LogVolCountMap;
LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
LogVolMap* LogVol;

G4RotationMatrix* RotY90=new G4RotationMatrix();
G4RotationMatrix* RotYM90=new G4RotationMatrix();

G4RotationMatrix* RotX90=new G4RotationMatrix();
G4RotationMatrix* RotXM90=new G4RotationMatrix();

G4RotationMatrix* RotYM90X90=new G4RotationMatrix();
G4RotationMatrix* RotYM90XM90=new G4RotationMatrix();

//G4Navigator* StepperNavigator;
//G4Navigator* QuadNavigator;

//=========================================
extern BDSOutput* bdsOutput;
extern G4bool verbose;
extern G4bool outline;

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif

//=================================================================



BDSDetectorConstruction::BDSDetectorConstruction():
  itsGeometrySampler(NULL),precisionRegion(NULL),gasRegion(NULL),
  solidWorld(NULL),logicWorld(NULL),physiWorld(NULL),
  magField(NULL),BDSUserLimits(NULL),BDSSensitiveDetector(NULL),
  itsIStore(NULL),_globalRotation(NULL)
{  
  verbose    = BDSExecOptions::Instance()->GetVerbose();
  outline    = BDSExecOptions::Instance()->GetOutline();
  gflash     = BDSExecOptions::Instance()->GetGFlash();
  gflashemax = BDSExecOptions::Instance()->GetGFlashEMax();
  gflashemin = BDSExecOptions::Instance()->GetGFlashEMin();
  
  //initialize world size vector
  itsWorldSize.push_back(0);
  itsWorldSize.push_back(1);
  itsWorldSize.push_back(2);

  //initialize global rotation matrix
  _globalRotation = new G4RotationMatrix();

// create commands for interactive definition of the beamline  
  G4double pi_ov_2 = asin(1.);

  RotY90->rotateY(pi_ov_2);
  RotYM90->rotateY(-pi_ov_2);
  RotX90->rotateX(pi_ov_2);
  RotXM90->rotateX(-pi_ov_2);
  RotYM90X90->rotateY(-pi_ov_2);
  RotYM90X90->rotateX( pi_ov_2);
  RotYM90XM90->rotateY(-pi_ov_2);
  RotYM90XM90->rotateX(-pi_ov_2);

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

#ifdef DEBUG
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
  theECList=new ECList;

  LogVolCount=new LogVolCountMap();

  LogVol=new LogVolMap();

  gasRegion = new G4Region("gasRegion");
  G4ProductionCuts* theGasProductionCuts = new G4ProductionCuts();
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("gamma"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e-"));
  theGasProductionCuts->SetProductionCut(1*CLHEP::m,G4ProductionCuts::GetIndex("e+"));
  gasRegion->SetProductionCuts(theGasProductionCuts);


  if (verbose || debug) G4cout << "-->starting BDS construction \n"<<G4endl;
  //Add the input sampler to the list of output sampler names
  //  bdsOutput->SampName.push_back((G4String)"input");
  //construct bds
  return ConstructBDS(beamline_list);
}


G4VPhysicalVolume* BDSDetectorConstruction::ConstructBDS(ElementList& beamline_list)
{
  //
  // set default output formats:
  //
  G4cout.precision(10);

  
  
  //
  // convert the parsed atom list to list of Geant4 G4Elements
  //
  list<struct Element>::iterator it;


  if (verbose || debug) G4cout << "parsing the atom list..."<< G4endl;
  for(it = atom_list.begin();it!=atom_list.end();it++)
  {
#ifdef DEBUG
    G4cout << "---->adding Atom, "
           << "name= " << (*it).name << " "
           << "symbol= " << (*it).symbol << " "
           << "Z= " << (*it).Z << " "
           << "A= " << (*it).A << "g/mole "
           << G4endl;
#endif

    BDSMaterials::Instance()->AddElement((*it).name,(*it).symbol,(*it).Z,(*it).A);
  }
  if (verbose || debug) G4cout << "size of atom list: "<< atom_list.size() << G4endl;


  //
  // convert the parsed material list to list of Geant4 G4Materials
  //
  if (verbose || debug) G4cout << "parsing the material list..."<< G4endl;
  for(it = material_list.begin();it!=material_list.end();it++)
  {
    if((*it).Z != 0) {
#ifdef DEBUG  
      G4cout << "---->adding Material, "
             << "name= "<< (*it).name << " "
             << "Z= " << (*it).Z << " "
             << "A= " << (*it).A << "g/mole "
             << "density= "<< (*it).density << "g/cm3 "
             << G4endl;
#endif
      BDSMaterials::Instance()->AddMaterial((*it).name,(*it).Z,(*it).A,(*it).density);
    }
    else if((*it).components.size() != 0){

      G4State itsState;
      if ((*it).state=="solid") itsState = kStateSolid;
      else
      if ((*it).state=="liquid") itsState = kStateLiquid;
      else
      if ((*it).state=="gas") itsState = kStateGas;
      else {
	G4cout << "Unknown material state "<< (*it).state 
	       << ", setting it to default (solid)"
	       << G4endl;
	(*it).state="solid";
	itsState = kStateSolid;
      }

      if((*it).componentsWeights.size()==(*it).components.size()) {

#ifdef DEBUG 
        G4cout << "---->adding Material, "
               << "name= "<< (*it).name << " "
               << "density= "<< (*it).density << "g/cm3 "
               << "state= " << (*it).state << " "
               << "T= " << (*it).temper << "K "
               << "P= " << (*it).pressure << "atm "
               << "ncomponents= " << (*it).components.size() << " "
               << G4endl;
#endif

	BDSMaterials::Instance()->AddMaterial((G4String)(*it).name,
				  (G4double)(*it).density,
				  (G4State)itsState,
				  (G4double)(*it).temper,
				  (G4double)(*it).pressure,
				  (std::list<const char*>)(*it).components,
				  (std::list<G4int>)(*it).componentsWeights);
      }
      else if((*it).componentsFractions.size()==(*it).components.size()) {

#ifdef DEBUG 
        G4cout << "---->adding Material, "
        << "name= "<< (*it).name << " "
        << "density= "<< (*it).density << "g/cm3 "
        << "state= " << (*it).state << " "
        << "T= " << (*it).temper << "K "
        << "P= " << (*it).pressure << "atm "
        << "ncomponents= " << (*it).components.size() << " "
        << G4endl;
#endif
        BDSMaterials::Instance()->AddMaterial((*it).name,
				  (*it).density,
				  itsState,
				  (*it).temper,
				  (*it).pressure,
				  (*it).components,
				  (*it).componentsFractions);
      }
      else {
	G4Exception("Badly defined material - number of components is not equal to number of weights or mass fractions!", "-1", FatalErrorInArgument, "");
	exit(1);
      }
    }
    else {
      G4Exception("Badly defined material - need more information!", "-1", FatalErrorInArgument, "");
      exit(1);
    }
  }
  if (verbose || debug) G4cout << "size of material list: "<< material_list.size() << G4endl;


  //
  // set global magnetic field first
  //
  SetMagField(0.0); // necessary to set a global field; so chose zero

  
  
  // convert the parsed element list to list of BDS elements
  //
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory();

  if (verbose || debug) G4cout << "parsing the beamline element list..."<< G4endl;
  for(it = beamline_list.begin();it!=beamline_list.end();it++){
#ifdef DEBUG
    G4cout << "BDSDetectorConstruction creating component..." << G4endl;
#endif
    BDSAcceleratorComponent* temp = theComponentFactory->createComponent(it, beamline_list);
#ifdef DEBUG
    G4cout << "pushing onto back of beamline..." << G4endl;
#endif
    if(temp){
      BDSBeamline::Instance()->addComponent(temp);
      //For the outline file...
      BDSBeamline::Instance()->currentItem()->SetK1((*it).k1);
      BDSBeamline::Instance()->currentItem()->SetK2((*it).k2);
      BDSBeamline::Instance()->currentItem()->SetK3((*it).k3);
    }
#ifdef DEBUG
    G4cout << "done." << G4endl;
#endif
  }
  delete theComponentFactory;
  theComponentFactory = NULL;

  G4cout << __METHOD_NAME__ << "size of beamline element list: "<< beamline_list.size() << G4endl;
  G4cout << __METHOD_NAME__ << "size of theBeamline: "<< BDSBeamline::Instance()->size() << G4endl;
    
  //
  // construct the component list
  //

  if (verbose || debug) G4cout << "now constructing geometry" << G4endl;
  
  list<BDSAcceleratorComponent*>::const_iterator iBeam;
  
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

  // define geometry scope
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    {

#ifdef DEBUG 
      G4cout << BDSBeamline::Instance()->currentItem()->GetName() << "  "
             << BDSBeamline::Instance()->currentItem()->GetLength() << "  "
             << BDSBeamline::Instance()->currentItem()->GetAngle() << "  "
             << G4endl;
#endif

      BDSBeamline::Instance()->currentItem()->SetSPos(s_tot+BDSBeamline::Instance()->currentItem()->GetArcLength()/2.0);

      // advance coordinates , but not for cylindrical sampler
      if(( BDSBeamline::Instance()->currentItem()->GetType() != "csampler") || ( BDSBeamline::Instance()->currentItem()->GetLength() <= BDSGlobalConstants::Instance()->GetSamplerLength() ) )
	{
	  s_tot+= BDSBeamline::Instance()->currentItem()->GetArcLength();

	  G4double angle=BDSBeamline::Instance()->currentItem()->GetAngle();
	  if(!angle && BDSBeamline::Instance()->currentItem()->GetType()=="transform3d")
	    angle=BDSBeamline::Instance()->currentItem()->GetPhi();
	  G4double theta=BDSBeamline::Instance()->currentItem()->GetTheta();
	  G4double psi=BDSBeamline::Instance()->currentItem()->GetPsi();

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
	  rtot += localZ * BDSBeamline::Instance()->currentItem()->GetZLength();
#ifdef DEBUG
          G4cout << BDSBeamline::Instance()->currentItem()->GetType() << " " << rtot << G4endl;
#endif
	}


      rextentmax = rtot + localX*BDSBeamline::Instance()->currentItem()->GetXLength() + localY*BDSBeamline::Instance()->currentItem()->GetYLength();
      rextentmin = rtot - localX*BDSBeamline::Instance()->currentItem()->GetXLength() - localY*BDSBeamline::Instance()->currentItem()->GetYLength();

      //      rextentmax(0) = rtot(0) + (localX*BDSBeamline::Instance()->currentItem()->GetXLength())(0) + (localX*BDSBeamline::Instance()->currentItem()->GetYLength())(0);
      //      rextentmax(1) = rtot(1) + (localY*BDSBeamline::Instance()->currentItem()->GetXLength())(1) + (localY*BDSBeamline::Instance()->currentItem()->GetYLength())(1);
      //      rextentmax(2) = rtot(2) + (localZ*BDSBeamline::Instance()->currentItem()->GetXLength())(2) + (localZ*BDSBeamline::Instance()->currentItem()->GetYLength())(2);

      //      rextentmin(0) = rtot(0) - localX*BDSBeamline::Instance()->currentItem()->GetXLength() - localX*BDSBeamline::Instance()->currentItem()->GetYLength();
      //      rextentmin(1) = rtot(1) - localY*BDSBeamline::Instance()->currentItem()->GetXLength() - localY*BDSBeamline::Instance()->currentItem()->GetYLength();
      //      rextentmin(2) = rtot(2) - localZ*BDSBeamline::Instance()->currentItem()->GetXLength() - localZ*BDSBeamline::Instance()->currentItem()->GetYLength();


	for(int i=0; i<3; i++){
	  rmax(i) = std::max(rextentmax(i),rmax(i));
	  rmin(i) = std::min(rextentmin(i),rmin(i));
	}

    }
    
  BDSGlobalConstants::Instance()->SetTotalS(s_tot);
  
  // -----------------------------------
  
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

  bdsOutput->zMax=s_tot;
  bdsOutput->transMax=std::max(GetWorldSizeX(), GetWorldSizeY());

  solidWorld = new G4Box("World", GetWorldSizeX(), GetWorldSizeY(), GetWorldSizeZ());
    
  logicWorld = new G4LogicalVolume(solidWorld,	       //its solid
				   BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()), //its material
				   "World");	       //its name
  
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);
  // set world volume visibility for debugging
#ifdef DEBUG 
  logicWorld->SetVisAttributes(new G4VisAttributes(true));	
#endif
	
  // set default max step length (only for particles which have the
  // G4StepLimiter process enabled)
#ifndef NOUSERLIMITS
  G4UserLimits* WorldUserLimits =new G4UserLimits();
  WorldUserLimits->SetMaxAllowedStep(10*CLHEP::m);
  WorldUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  WorldUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  logicWorld->SetUserLimits(WorldUserLimits);
#endif


#ifdef DEBUG
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

  G4bool use_graphics=true;
  G4ThreeVector TargetPos;

  // set default output formats:
  G4cout.precision(15);
  
#ifdef DEBUG 
  G4cout<<"total length="<<s_tot/CLHEP::m<<"m"<<G4endl;
#endif
  
  // reset counters:
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next()){

    // zero length components have no logical volumes
    if(BDSBeamline::Instance()->currentItem()->GetLength()!=0.)
      {
	G4String logVolName = BDSBeamline::Instance()->currentItem()->GetMarkerLogicalVolume()->GetName();
	(*LogVolCount)[logVolName]=1;
      }
  }
  
  if (verbose || debug) G4cout<<"starting placement procedure "<<G4endl;
  
  rtot = G4ThreeVector(0.,0.,0.);
  localX = G4ThreeVector(1.,0.,0.); 
  localY = G4ThreeVector(0.,1.,0.);
  localZ = G4ThreeVector(0.,0.,1.);
  
  
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    { 
      //BDSBeamline::Instance()->currentItem()->SetZLower(rtot.z());
       
      G4double angle=BDSBeamline::Instance()->currentItem()->GetAngle();
      G4double theta=BDSBeamline::Instance()->currentItem()->GetTheta();
      G4double psi = BDSBeamline::Instance()->currentItem()->GetPsi();
      G4double tilt = BDSBeamline::Instance()->currentItem()->GetTilt();
      G4double phi = BDSBeamline::Instance()->currentItem()->GetPhi();
      G4double length = BDSBeamline::Instance()->currentItem()->GetZLength();

      if( BDSBeamline::Instance()->currentItem()->GetType() == "transform3d")
	{

#ifdef DEBUG 
          G4cout<<"transform3d : "<<phi<<" "<<theta<<" "<<psi<<G4endl;
#endif


	  rtot(0) += BDSBeamline::Instance()->currentItem()->GetXOffset(); 
	  rtot(1) += BDSBeamline::Instance()->currentItem()->GetYOffset(); 
	  rtot(2) += BDSBeamline::Instance()->currentItem()->GetZOffset(); 

	  rlast(0) += BDSBeamline::Instance()->currentItem()->GetXOffset();
	  rlast(1) += BDSBeamline::Instance()->currentItem()->GetYOffset(); 
	  rlast(2) += BDSBeamline::Instance()->currentItem()->GetZOffset(); 

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
      if(BDSBeamline::Instance()->currentItem()->GetType() == "sbend" || BDSBeamline::Instance()->currentItem()->GetType() == "rbend" )
	{
	  _globalRotation->rotate(tilt,localZ);
	  localX.rotate(tilt,localZ);
	  localY.rotate(tilt,localZ);
	}
      else 
	rotateComponent->rotateZ(tilt);
    
      // define center of bended elements from the previos coordinate frame
      G4ThreeVector zHalfAngle = localZ; 

      if( BDSBeamline::Instance()->currentItem()->GetType() == "sbend" || BDSBeamline::Instance()->currentItem()->GetType() == "rbend"  )
	zHalfAngle.rotate(angle/2,localY);

#ifdef DEBUG
      G4cout<<"zHalfAngle="<<zHalfAngle<<G4endl;
      G4cout<<"localZ="<<localZ<<G4endl;
      G4cout<<"localX="<<localX<<G4endl;
      G4cout<<"localY="<<localY<<G4endl;
      G4cout<<"rlast="<<rlast<<G4endl;
#endif
      
      // target position
      TargetPos = rlast + zHalfAngle *  ( length/2 + BDSGlobalConstants::Instance()->GetLengthSafety()/2 ) ;

#ifdef DEBUG 
      G4cout<<"TargetPos="<<TargetPos<<G4endl;
#endif

      // advance the coordinates, but not for cylindrical samplers 
      if( ( ( BDSBeamline::Instance()->currentItem()->GetType() != "csampler") || ( length <= BDSGlobalConstants::Instance()->GetSamplerLength() ) )  && ( BDSBeamline::Instance()->currentItem()->GetType()!=_ELEMENT ))
	{
#ifdef DEBUG 
          G4cout << BDSBeamline::Instance()->currentItem()->GetType() << " "
                 << BDSBeamline::Instance()->currentItem()->GetName() << " "
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
      if( BDSBeamline::Instance()->currentItem()->GetType() == "sbend" || BDSBeamline::Instance()->currentItem()->GetType() == "rbend")
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
	if (BDSBeamline::Instance()->currentItem()->GetMarkerLogicalVolume()->GetSolid()->GetName().contains("trapezoid") ){
	  rotateComponent->rotateY(-CLHEP::twopi/4); //Drift trapezoids defined along z axis 
	}
      }
    
    					

      // zero length components not placed (transform3d)
      if(length!=0.){
	G4LogicalVolume* LocalLogVol=BDSBeamline::Instance()->currentItem()->GetMarkerLogicalVolume();
	
	G4String LogVolName=LocalLogVol->GetName();
	//--test
	G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
	VisAtt1->SetForceSolid(true);  
	VisAtt1->SetVisibility(false);
	LocalLogVol->SetVisAttributes(VisAtt1);
	//------------
	int nCopy=(*LogVolCount)[LogVolName]-1;
	(*LogVolCount)[LogVolName]++;


	// add the wolume to one of the regions
	if(BDSBeamline::Instance()->currentItem()->GetPrecisionRegion())
	  {
#ifdef DEBUG
	    G4cout<<"ELEMENT IS IN PRECISION REGION: "<<BDSBeamline::Instance()->currentItem()->GetPrecisionRegion()<< G4endl;
#endif
	    LocalLogVol->SetRegion(precisionRegion);
	    precisionRegion->AddRootLogicalVolume(LocalLogVol);
	  }

	
#ifdef DEBUG
	G4cout<<"SETTING UP SENSITIVE VOLUME..."<< G4endl;
#endif	
	// set up the sensitive volumes for energy counting:
	BDSBeamline::Instance()->currentItem()->SetCopyNumber(nCopy);
	G4LogicalVolume* SensVol=BDSBeamline::Instance()->currentItem()->GetSensitiveVolume();

	if(SensVol)
	  {
	    BDSEnergyCounterSD* ECounter=new BDSEnergyCounterSD(LogVolName);
	    BDSBeamline::Instance()->currentItem()->SetBDSEnergyCounter(ECounter);
	    SensVol->SetSensitiveDetector(ECounter);
	    SDman->AddNewDetector(ECounter);
	    theECList->push_back(ECounter);
	  }


#ifdef DEBUG
	G4cout<<"SETTING UP MULTIPLE SENSITIVE VOLUMES..."<< G4endl;
#endif	
	vector<G4LogicalVolume*> MultipleSensVols = BDSBeamline::Instance()->currentItem()->GetMultipleSensitiveVolumes();
	if( ( BDSBeamline::Instance()->currentItem()->GetType()!="sampler" && BDSBeamline::Instance()->currentItem()->GetType()!="csampler" )
	    && MultipleSensVols.size()>0)
	  {
	    for(G4int i=0; i<(G4int)MultipleSensVols.size(); i++)
	      {
		BDSEnergyCounterSD* ECounter=
		  new BDSEnergyCounterSD(LogVolName+BDSGlobalConstants::Instance()->StringFromInt(i));
		BDSBeamline::Instance()->currentItem()->SetBDSEnergyCounter(ECounter);
		MultipleSensVols.at(i)->SetSensitiveDetector(ECounter);
		SDman->AddNewDetector(ECounter);
		theECList->push_back(ECounter);	     
		
		if(gflash){
		  if((MultipleSensVols.at(i)->GetRegion() != precisionRegion) && (BDSBeamline::Instance()->currentItem()->GetType()==_ELEMENT)){//If not in the precision region....
		    //		    if(MultipleSensVols[i]->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
#ifdef DEBUG
		    G4cout << "...adding " << MultipleSensVols[i]->GetName() << " to gFlashRegion" << G4endl;
#endif
		    /**********************************************                                                                                                                       
		     * Initialise shower model                                                                                                                                          
		     ***********************************************/
		    G4String rname = "gFlashRegion_" + MultipleSensVols[i]->GetName();
		    gFlashRegion.push_back(new G4Region(rname.c_str()));
		    G4String mname = "fastShowerModel" + rname;
#ifdef DEBUG
		    G4cout << "...making parameterisation..." << G4endl;
#endif
		    theFastShowerModel.push_back(new BDSShowerModel(mname.c_str(),gFlashRegion.back()));
		    theParameterisation.push_back(new GFlashHomoShowerParameterisation(BDSMaterials::Instance()->GetMaterial(MultipleSensVols[i]->GetMaterial()->GetName().c_str()))); 
		    theFastShowerModel.back()->SetParameterisation(*theParameterisation.back());
		    theFastShowerModel.back()->SetParticleBounds(*theParticleBounds) ;
		    theFastShowerModel.back()->SetHitMaker(*theHitMaker);
		    if(MultipleSensVols[i]->GetMaterial()->GetState()!=kStateGas){ //If the region material state is not gas, associate with a parameterisation
		      theFastShowerModel.back()->SetFlagParamType(1);//Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
		      theFastShowerModel.back()->SetFlagParticleContainment(1);//Turn on containment
		    } else {
		      theFastShowerModel.back()->SetFlagParamType(0);//Turn on the parameterisation for e-m showers starting in sensitive material and fitting in the current stack.
		      theFastShowerModel.back()->SetFlagParticleContainment(0);//Turn on containment

		    }
		    MultipleSensVols[i]->SetRegion(gFlashRegion.back());
		    gFlashRegion.back()->AddRootLogicalVolume(MultipleSensVols[i]);
		    //		    gFlashRegion.back()->SetUserLimits(new G4UserLimits(BDSBeamline::Instance()->currentItem()->GetLength()/10.0));
		    //		    MultipleSensVols[i]->SetUserLimits(new G4UserLimits(BDSBeamline::Instance()->currentItem()->GetLength()/10.0));
		  }		  
		}
	      }

	  }


	    //Loop through again, unsetting gas regions

	//	if(MultipleSensVols.size()>0){
	//	  for(G4int i=0; i<(G4int)MultipleSensVols.size(); i++){
	//	    if(MultipleSensVols[i]->GetMaterial()->GetState()==kStateGas){ //If the region material state is not gas, associate with a parameterisation
	//	      MultipleSensVols[i]->GetRegion()->RemoveRootLogicalVolume(MultipleSensVols[i]);
	      //	      MultipleSensVols[i]->SetRegion(NULL);

	      //	      MultipleSensVols[i]->SetRegion(gasRegion);
	      //	      MultipleSensVols[i]->SetUserLimits(new G4UserLimits(BDSBeamline::Instance()->currentItem()->GetLength()/10.0));
	//      }
	//      }
	//      }

	
	
  
	if(BDSBeamline::Instance()->currentItem()->GetType()=="sampler") {
	  LocalName=BDSBeamline::Instance()->currentItem()->GetName()+"_phys";
	  bdsOutput->SampName.push_back(LocalName + "_" + BDSGlobalConstants::Instance()->StringFromInt(nCopy+1));
	} 
	else if(BDSBeamline::Instance()->currentItem()->GetType()=="csampler") {
	  LocalName=BDSBeamline::Instance()->currentItem()->GetName()+"_phys";
	  bdsOutput->CSampName.push_back(LocalName + "_" + BDSGlobalConstants::Instance()->StringFromInt(nCopy+1));
	} else {
	  //it would be nice to set correctly names also for other elements...
	  //but need to count them!
	  LocalName=BDSBeamline::Instance()->currentItem()->GetName()+"_phys";
	}

	/*
	//for torus sbend
	if(BDSBeamline::Instance()->currentItem()->GetType() == "sbend") {

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

#ifdef DEBUG
	G4cout<<"ALIGNING COMPONENT..."<< G4endl;
#endif	
	// Align Component - most cases does nothing. 
	// Currently only used for BDSElement	
	// For other elements stores global position and rotation,
	// needed for BDSOutline
	BDSBeamline::Instance()->currentItem()->AlignComponent(//TargetPos,
				 rlast,
				 rotateComponent,
				 *_globalRotation,
				 rtot,
				 rlast,
				 localX,
				 localY,
				 localZ);

#ifdef DEBUG
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
	  vector<G4VPhysicalVolume*> MultiplePhysicalVolumes = BDSBeamline::Instance()->currentItem()->GetMultiplePhysicalVolumes();
	  for (unsigned int i=0;i<MultiplePhysicalVolumes.size(); i++) fPhysicalVolumeVector.push_back(MultiplePhysicalVolumes.at(i));
					    

	  
#ifdef DEBUG 
        G4cout << "Volume name: " << LocalName << G4endl;
#endif
        if(BDSGlobalConstants::Instance()->GetRefVolume()+"_phys"==LocalName && 
           BDSGlobalConstants::Instance()->GetRefCopyNo()==nCopy){
#ifdef DEBUG 
          G4cout << "Setting new transform" <<G4endl;
#endif
	  G4AffineTransform tf(*_globalRotation,TargetPos-G4ThreeVector(0,0,length/2));
	  BDSGlobalConstants::Instance()->SetRefTransform(tf);
        }

	BDSBeamline::Instance()->currentItem()->PrepareField(PhysiComponentPlace);

	if(use_graphics)
	  {
	    	    BDSBeamline::Instance()->currentItem()->GetVisAttributes()->SetVisibility(true);
#ifdef DEBUG
		    BDSBeamline::Instance()->currentItem()->GetMarkerLogicalVolume()->
	    	      SetVisAttributes(BDSBeamline::Instance()->currentItem()->GetVisAttributes());
#endif
	  }
	
      }
    }

  // construct tunnel
  for(it = beamline_list.begin();it!=beamline_list.end();it++)
    {
      
      if((*it).type==_TUNNEL ) {
#ifdef DEBUG
	G4cout<<"BUILDING TUNNEL : "<<(*it).l<<"  "<<(*it).name<<G4endl;
#endif
	
	G4String gFormat="",  GFile="";
	G4String geometry = (*it).geometryFile;

	// get geometry format and file
	G4int pos = geometry.find(":");
	
	if(pos<0) { 
	  G4cerr<<"WARNING: invalid geometry reference format : "<<geometry<<endl;
	  gFormat="none";
	}
	
	else {
	  gFormat = geometry.substr(0,pos);
	  GFile = geometry.substr(pos+1,geometry.length() - pos); 
	}
	
#ifdef DEBUG
	G4cout<<"placing components\n: geometry format - "<<gFormat<<G4endl<<
	  "file - "<<GFile<<G4endl;
#endif
	
	if(gFormat=="gmad") {
	 
	  GGmadDriver ggmad(GFile);
	  ggmad.Construct(logicWorld);
	  
	} else  G4cerr<<"Tunnel won't be build! "<<endl;
      }
      
    }
  
  // free the parser list
  for(it = beamline_list.begin();it!=beamline_list.end();it++) {
    delete (*it).lst;
  }
  beamline_list.clear();

  if(verbose || debug) G4cout<<"end placement, size="<<BDSBeamline::Instance()->size()<<G4endl;
  
  if(verbose || debug) G4cout<<"Detector Construction done"<<G4endl; 

#ifdef DEBUG 
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
#endif

    if(verbose || debug) G4cout<<"Finished listing materials, returning physiWorld"<<G4endl; 

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
  
void BDSDetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructBDS(beamline_list));
}

//=================================================================
BDSDetectorConstruction::~BDSDetectorConstruction()
{ 
  LogVolCount->clear();
  delete LogVolCount;

  LogVolMap::iterator iter;
  for(iter=LogVol->begin();iter!=LogVol->end();iter++){
    delete (*iter).second;
  }
  LogVol->clear();
  delete LogVol;

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

void BDSDetectorConstruction::SetWorldSize(G4double* val){
  int sExpected = 3;
  int s=sizeof(val)/sizeof(val[0]);
  if(s!=sExpected){
    std::cerr << "Error: BDSDetectorConstruction::SetWorldSize(G4double*) expects an array of size " << sExpected << ". Exiting." << std::endl;
    exit(1);
  }
  for(int i=0; i<s; i++){
    itsWorldSize[i]=val[i];
  }
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
