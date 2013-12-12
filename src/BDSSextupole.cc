/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to be the BDSGlobal version
*/

#include "BDSGlobalConstants.hh" 

#include "BDSSextupole.hh"
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

BDSSextupole::BDSSextupole(G4String aName, G4double aLength, 
			   G4double bpRad, G4double FeRad,
			   G4double BDblPrime, G4double tilt, 
			   G4double outR, 
                           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                           G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBDblPrime(BDblPrime),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsType="sext";

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildDefaultMarkerLogicalVolume();

      //
      //build tunnel
      //
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
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
      if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  //coordinate in GetFieldValue
	  polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(pi/6);
	  polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(pi/6);
	  polePos[2]=0.;
	  polePos[3]=-999.;//flag to use polePos rather than local track

	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	    sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	    BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	    (BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	     BDSGlobalConstants::Instance()->GetMagnetPoleRadius());

	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;

	  BuildOuterFieldManager(6, BFldIron,pi/6);
	}
      //Build the beam loss monitors
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
      if(BDSGlobalConstants::Instance()->GetSynchRadOn()&& BDSGlobalConstants::Instance()->GetSynchRescale())
	{
	  // with synchrotron radiation, the rescaled magnetic field
	  // means elements with the same name must have different
	  //logical volumes, becuase they have different fields
	  itsName+=BDSGlobalConstants::Instance()->StringFromInt((*LogVolCount)[itsName]);

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
	      G4double polePos[4];
	      G4double Bfield[3];
	      
	      //coordinate in GetFieldValue
	      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(pi/6);
	      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(pi/6);
	      polePos[2]=0.;
	      polePos[3]=-999.;//flag to use polePos rather than local track

	      itsMagField->GetFieldValue(polePos,Bfield);
	      G4double BFldIron=
		sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
		BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
		(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
		 BDSGlobalConstants::Instance()->GetMagnetPoleRadius());

	      // Magnetic flux from a pole is divided in two directions
	      BFldIron/=2.;
	      
	      BuildOuterFieldManager(6, BFldIron,pi/6);
	    }
	  //When is SynchRescale(factor) called?

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

void BDSSextupole::SynchRescale(G4double factor)
{
  itsStepper->SetBDblPrime(factor*itsBDblPrime);
  itsMagField->SetBDblPrime(factor*itsBDblPrime);
#ifdef DEBUG 
  G4cout << "Sext " << itsName << " has been scaled" << G4endl;
#endif
}

G4VisAttributes* BDSSextupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,0));
  return itsVisAttributes;
}


void BDSSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSSextMagField(1*itsBDblPrime); //L Deacon testing field sign 4/7/12
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);

  itsStepper=new BDSSextStepper(itsEqRhs);
  itsStepper->SetBDblPrime(itsBDblPrime);
}

BDSSextupole::~BDSSextupole()
{
  delete itsVisAttributes;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
