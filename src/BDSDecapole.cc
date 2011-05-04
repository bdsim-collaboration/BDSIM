/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to be the BDSGlobal version
*/

#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSDecapole.hh"
#include "G4Box.hh"
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

extern BDSMaterials* theMaterials;
//============================================================

BDSDecapole::BDSDecapole(G4String aName, G4double aLength, 
			 G4double bpRad, G4double FeRad,
			 G4double BQuadPrime, G4double tilt, 
			 G4double outR, 
                         std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                         G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBQuadPrime(BQuadPrime)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsType="deca";

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildDefaultMarkerLogicalVolume();

      //
      //build tunnel
      //
      if(BDSGlobals->GetBuildTunnel()){
        BuildTunnel();
      }

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
      if(BDSGlobals->GetIncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  //coordinate in GetFieldValue
	  polePos[0]=-BDSGlobals->GetMagnetPoleRadius()*sin(pi/10);
	  polePos[1]=BDSGlobals->GetMagnetPoleRadius()*cos(pi/10);
	  polePos[2]=0.;
	  polePos[3]=-999.;//flag to use polePos rather than local track

	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	    sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	    BDSGlobals->GetMagnetPoleSize()/
	    (BDSGlobals->GetComponentBoxSize()/2-
	     BDSGlobals->GetMagnetPoleRadius());

	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;

	  BuildOuterFieldManager(10, BFldIron,pi/10);
	}
      //Build the beam loss monitors
      BuildBLMs();
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
	  BuildBeampipe();

	  //
	  // build magnet (geometry + magnetic field)
	  //
	  BuildDefaultOuterLogicalVolume(itsLength);
	  if(BDSGlobals->GetIncludeIronMagFields())
	    {
	      G4double polePos[4];
	      G4double Bfield[3];
	      
	      //coordinate in GetFieldValue
	      polePos[0]=-BDSGlobals->GetMagnetPoleRadius()*sin(pi/10);
	      polePos[1]=BDSGlobals->GetMagnetPoleRadius()*cos(pi/10);
	      polePos[2]=0.;
	      polePos[3]=-999.;//flag to use polePos rather than local track

	      itsMagField->GetFieldValue(polePos,Bfield);
	      G4double BFldIron=
		sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
		BDSGlobals->GetMagnetPoleSize()/
		(BDSGlobals->GetComponentBoxSize()/2-
		 BDSGlobals->GetMagnetPoleRadius());

	      // Magnetic flux from a pole is divided in two directions
	      BFldIron/=2.;
	      
	      BuildOuterFieldManager(10, BFldIron,pi/10);
	    }
	  //When is SynchRescale(factor) called?

	  //
	  // define sensitive volumes for hit generation
	  //
          if(BDSGlobals->GetSensitiveBeamPipe()){
            SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
          }
          if(BDSGlobals->GetSensitiveComponents()){
            SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
	  }
	  //
	  // set visualization attributes
	  //
	  itsVisAttributes=SetVisAttributes();
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

void BDSDecapole::SynchRescale(G4double factor)
{
  itsStepper->SetBQuadPrime(factor*itsBQuadPrime);
  itsMagField->SetBQuadPrime(factor*itsBQuadPrime);
#ifdef DEBUG
  G4cout << "Dec " << itsName << " has been scaled" << G4endl;
#endif
}

G4VisAttributes* BDSDecapole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,0,1)); //green
  return itsVisAttributes;
}

void BDSDecapole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSDecMagField(itsBQuadPrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  itsStepper=new BDSDecStepper(itsEqRhs);
  itsStepper->SetBQuadPrime(itsBQuadPrime);
}

BDSDecapole::~BDSDecapole()
{
  delete itsVisAttributes;
  delete itsMarkerLogicalVolume;
  delete itsOuterLogicalVolume;
  delete itsPhysiComp;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
