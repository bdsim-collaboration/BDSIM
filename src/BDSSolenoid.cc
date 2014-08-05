//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.4
//  
//
//
//   Solenoid class
//
//
//   History
//
//     21 Oct 2007 by Marchiori,  v.0.4
//
//


#include "BDSGlobalConstants.hh" 

#include "BDSSolenoid.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSSolenoidMagField.hh"
#include "G4MagneticField.hh"
#include "BDSSolenoidStepper.hh"
#include "G4HelixImplicitEuler.hh"

#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSSolenoid::BDSSolenoid(G4String aName, G4double aLength, 
			 G4double bpRad, G4double FeRad,
			 G4double bField, G4double outR,
                         std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
			 G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBField(bField),
  itsMagField(NULL),itsStepper(NULL),itsEqRhs(NULL)
{
  SetOuterRadius(outR);
  itsType="solenoid";
 
  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildDefaultMarkerLogicalVolume();

      //
      // build beampipe (geometry + magnetic field)
      //
      BuildBPFieldAndStepper();
      BuildBPFieldMgr(itsStepper,itsMagField);
      BuildBeampipe();

      //
      // build magnet (geometry + magnetic field)
      //
      BuildDefaultOuterLogicalVolume(itsLength);
      if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
	{
	  G4cerr<<"IncludeIronMagFields option not implemented for solenoid class"<<G4endl;
	}

      BuildBLMs();

      //
      // define sensitive volumes for hit generation
      //
      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
        SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      }

      //
      // set visualization attributes
      //
      itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

      //
      // append marker logical volume to volume map
      //
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
	  //
	  // use already defined marker volume
	  //
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}
  
G4VisAttributes* BDSSolenoid::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1.,0.,0.)); //red
  itsVisAttributes->SetForceSolid(true);
  return itsVisAttributes;
}

void BDSSolenoid::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper

#ifdef _USE_GEANT4_STEPPER_
  // using Geant4
  itsMagField = new BDSSolenoidMagField(itsBField);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper=new G4HelixImplicitEuler(itsEqRhs);
#else
  // using BDSIM
  G4ThreeVector Bfield(0.,0.,itsBField);
  itsMagField=new G4UniformMagField(Bfield);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper=new BDSSolenoidStepper(itsEqRhs);
  itsStepper->SetBField(itsBField);
#endif
}

BDSSolenoid::~BDSSolenoid()
{
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
