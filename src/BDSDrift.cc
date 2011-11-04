/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSDrift.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4CashKarpRKF45.hh"

#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern BDSMaterials* theMaterials;
//============================================================

BDSDrift::BDSDrift (G4String aName, G4double aLength, 
                    std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, G4double aperX, G4double aperY, G4String tunnelMaterial, G4bool aperset, G4double aper, G4double tunnelOffsetX):
  BDSMultipole(aName, aLength, aper, aper, SetVisAttributes(),  blmLocZ, blmLocTheta, tunnelMaterial, "", aperX, aperY, 0, 0, tunnelOffsetX)
{
  if(!aperset){
    itsStartOuterR=aperX + BDSGlobals->GetBeampipeThickness();
    itsEndOuterR=aperY + BDSGlobals->GetBeampipeThickness();
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

      //
      // build beampipe (geometry + magnetic field)
      //
      if(BDSGlobals->GetBuildTunnel()){
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
      if(BDSGlobals->GetSensitiveBeamPipe()){
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
#ifndef NODRIFTSTEPPER
  itsStepper=new BDSDriftStepper(itsEqRhs);
#else
  itsStepper = new G4CashKarpRKF45(itsEqRhs); //For constant magnetic field
#endif
}

void BDSDrift::BuildBLMs(){
  itsBlmLocationR = std::max(itsStartOuterR, itsEndOuterR) - itsBpRadius;
  BDSAcceleratorComponent::BuildBLMs();
}

BDSDrift::~BDSDrift()
{
  if(itsVisAttributes) delete itsVisAttributes;
  if(itsMarkerLogicalVolume) delete itsMarkerLogicalVolume;
  if(itsOuterLogicalVolume) delete itsOuterLogicalVolume;
  if(itsPhysiComp) delete itsPhysiComp;
  //if(itsField) delete itsField;
  //if(itsEqRhs) delete itsEqRhs;
  //if(itsStepper) delete itsStepper;
}
