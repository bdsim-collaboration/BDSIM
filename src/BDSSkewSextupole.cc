/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to BDSGlobalConstants::Instance() version
*/
#include "BDSGlobalConstants.hh" 

#include "BDSSkewSextupole.hh"
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

BDSSkewSextupole::BDSSkewSextupole(G4String& aName,G4double aLength, 
				   G4double bpRad,G4double FeRad,
                                   std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                                   G4String aTunnelMaterial, G4String aMaterial,
				   G4double BDblPrime):
  BDSMultipole(aName,aLength, bpRad, FeRad,SetVisAttributes(),blmLocZ, blmLocTheta, aTunnelMaterial,aMaterial),
  itsBDblPrime(BDblPrime),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  if (!(*LogVolCount)[itsName])
    {
      BuildBPFieldAndStepper();
      BuildBPFieldMgr(itsStepper,itsMagField);
      BuildDefaultMarkerLogicalVolume();

      BuildBeampipe();

      BuildDefaultOuterLogicalVolume(itsLength);

      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
        SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      }

      if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/6);
	  polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/6);
	  polePos[2]=0.;
	  polePos[3]=-999.;//flag to use polePos rather than local track
	                   //coordinate in GetFieldValue	    
	    
	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	    sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	    BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	    (BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	     BDSGlobalConstants::Instance()->GetMagnetPoleRadius());
	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;

	  BuildOuterFieldManager(6, BFldIron,CLHEP::pi/6);
	}


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
	  BuildBPFieldAndStepper();
	  BuildBPFieldMgr(itsStepper,itsMagField);
	  BuildDefaultMarkerLogicalVolume();
	  
          if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
            BuildTunnel();
          }
          BuildBeampipe();
	  BuildDefaultOuterLogicalVolume(itsLength);

          //Build the beam loss monitors
          BuildBLMs();

          if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
            SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
          }
          if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
            SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
          }

	  (*LogVol)[itsName]=itsMarkerLogicalVolume;
	}
      else
	{
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
	}      
    }
  
}


G4VisAttributes* BDSSkewSextupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,0));
  return itsVisAttributes;
}


void BDSSkewSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSSkewSextMagField(itsBDblPrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  itsStepper=new BDSSkewSextStepper(itsEqRhs);
  itsStepper->SetBDblPrime(itsBDblPrime);

}

BDSSkewSextupole::~BDSSkewSextupole()
{
  delete itsVisAttributes;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
