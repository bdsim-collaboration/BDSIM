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


#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSSolenoid.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSSolenoidMagField.hh"
#include "G4MagneticField.hh"
#include "BDSSolenoidStepper.hh"
#include "G4HelixImplicitEuler.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include <map>

const int DEBUG = 0;

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern BDSMaterials* theMaterials;
//============================================================

BDSSolenoid::BDSSolenoid(G4String aName, G4double aLength, 
			 G4double bpRad, G4double FeRad,
			 G4double bField, G4double outR,
			 G4String aMaterial, G4String spec):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), aMaterial),
  itsBField(bField)
{
  SetOuterRadius(outR);
  itsType="solenoid";
 
  if(DEBUG) G4cout<<"BDSSOLENOID : SPEC : "<<spec<<G4endl;

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
      BuildBeampipe(itsLength);

      //
      // build magnet (geometry + magnetic field)
      //
      BuildDefaultOuterLogicalVolume(itsLength);
      if(BDSGlobals->GetIncludeIronMagFields())
	{
	  G4cerr<<"IncludeIronMagFields option not implemented for solenoid class"<<G4endl;
	}

      //
      // define sensitive volumes for hit generation
      //
      SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      SetMultipleSensitiveVolumes(itsOuterLogicalVolume);

      //
      // set visualization attributes
      //
      itsVisAttributes=SetVisAttributes();
      itsVisAttributes->SetForceSolid(true);
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
      if(BDSGlobals->GetSynchRadOn()&& BDSGlobals->GetSynchRescale())
	{
	  // with synchrotron radiation, the rescaled magnetic field
	  // means elements with the same name must have different
	  //logical volumes, becuase they have different fields
	  itsName+=BDSGlobals->StringFromInt((*LogVolCount)[itsName]);

	  //
	  // build external volume
	  // 
	  BuildDefaultMarkerLogicalVolume();

	  //
	  // build beampipe (geometry + magnetic field)
	  //
	  BuildBPFieldAndStepper();
	  BuildBPFieldMgr(itsStepper,itsMagField);
	  BuildBeampipe(itsLength);

	  //
	  // build magnet (geometry + magnetic field)
	  //
	  BuildDefaultOuterLogicalVolume(itsLength);
	  if(BDSGlobals->GetIncludeIronMagFields())
	    {
	      G4cerr<<"IncludeIronMagFields option not implemented for solenoid class"<<G4endl;
	    }
	  //When is SynchRescale(factor) called?

	  //
	  // define sensitive volumes for hit generation
	  //
	  SetSensitiveVolume(itsBeampipeLogicalVolume);// for synchrotron
	  //SetSensitiveVolume(itsOuterLogicalVolume);// for laserwire      
	  
	  //
	  // set visualization attributes
	  //
	  itsVisAttributes=new G4VisAttributes(G4Colour(1.,0.,0.)); //red
	  itsVisAttributes->SetForceSolid(true);
	  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);
	  
	  //
	  // append marker logical volume to volume map
	  //
	  (*LogVol)[itsName]=itsMarkerLogicalVolume;
	}
      else
	{
	  //
	  // use already defined marker volume
	  //
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
	}      
    }
}
  
void BDSSolenoid::SynchRescale(G4double factor)
{
#ifdef _USE_GEANT4_STEPPER_
  itsMagField->SetBField(factor*itsBField);
#else
  itsStepper->SetBField(factor*itsBField);
  itsMagField->SetFieldValue(G4ThreeVector(0.0,0.0,factor*itsBField));
#endif
  if(DEBUG) G4cout << "Solenoid " << itsName << " has been scaled" << G4endl;
}

G4VisAttributes* BDSSolenoid::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1.,0.,0.)); //red
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
  delete itsVisAttributes;
  delete itsMarkerLogicalVolume;
  delete itsOuterLogicalVolume;
  delete itsPhysiComp;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
