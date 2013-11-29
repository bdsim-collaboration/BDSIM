/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSDrift.hh"
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

BDSDrift::BDSDrift (G4String aName, G4double aLength, 
                    std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, G4double aperX, G4double aperY, G4String tunnelMaterial, G4bool aperset, G4double aper, G4double tunnelOffsetX, G4double phiAngleIn, G4double phiAngleOut):
  BDSMultipole(aName, aLength, aper, aper, SetVisAttributes(),  blmLocZ, blmLocTheta, tunnelMaterial, "", aperX, aperY, 0, 0, tunnelOffsetX, phiAngleIn, phiAngleOut),
  itsStartOuterR(0.0),itsEndOuterR(0.0),itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  if(!aperset){
    itsStartOuterR=aperX + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    itsEndOuterR=aperY + BDSGlobalConstants::Instance()->GetBeampipeThickness();
    SetStartOuterRadius(itsStartOuterR);
    SetEndOuterRadius(itsEndOuterR);
  }
  itsType="drift";

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildDefaultMarkerLogicalVolume();
      G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
      VisAtt1->SetVisibility(false);
      VisAtt1->SetForceSolid(true);
      itsMarkerLogicalVolume->SetVisAttributes(VisAtt1);

      //
      // build beampipe (geometry + magnetic field)
      //
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
        BuildTunnel();
      }

      BuildBpFieldAndStepper();
      BuildBPFieldMgr(itsStepper, itsMagField);
      if (aperset){
	BuildBeampipe();
      } else {
	BuildBeampipe(aperX, aperY);
      }
      BuildBLMs();
  
      //
      // define sensitive volumes for hit generation
      //
      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      
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

G4VisAttributes* BDSDrift::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,1,0)); //useless
  return itsVisAttributes;
}

void BDSDrift::BuildBpFieldAndStepper(){
    // set up the magnetic field and stepper
  itsMagField=new BDSMagField(); //Zero magnetic field.
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper=new BDSDriftStepper(itsEqRhs);
}

void BDSDrift::BuildBLMs(){
  itsBlmLocationR = std::max(itsStartOuterR, itsEndOuterR) - itsBpRadius;
  BDSAcceleratorComponent::BuildBLMs(); // resets itsBlmLocationR! -- JS
}

BDSDrift::~BDSDrift()
{
  delete itsVisAttributes;
//   //  delete itsPhysiComp;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
