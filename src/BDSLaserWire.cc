/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSLaserWire.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"

#include <map>

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSLaserWire::BDSLaserWire (G4String aName,G4double aLength,
G4double aWavelength, G4ThreeVector aDirection):
  BDSAcceleratorComponent(
    aName,
    aLength,0,0,0,
    SetVisAttributes()),
  itsLaserCompton(NULL),
  itsLaserDirection(aDirection),itsLaserWavelength(aWavelength)
{
  LaserWireLogicalVolume();
  BDSGlobalConstants::Instance()->
    SetLaserwireWavelength(itsMarkerLogicalVolume->GetName(),aWavelength);
  BDSGlobalConstants::Instance()->
    SetLaserwireDir(itsMarkerLogicalVolume->GetName(),aDirection);
}

void BDSLaserWire::LaserWireLogicalVolume()
{
  if(!(*LogVolCount)[itsName])
    {
      itsMarkerLogicalVolume=new G4LogicalVolume(
						 new G4Box(itsName+"_solid",
							   BDSGlobalConstants::Instance()->
							   GetBeampipeRadius(),
							   BDSGlobalConstants::Instance()->
							   GetBeampipeRadius(),
							   itsLength/2),
						 BDSMaterials::Instance()->GetMaterial("LaserVac"),
						 itsName);
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
  itsMarkerLogicalVolume->SetVisAttributes(itsVisAttributes);
}

G4VisAttributes* BDSLaserWire::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.,1.,0.));
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
  return itsVisAttributes;
}

BDSLaserWire::~BDSLaserWire()
{
  delete itsLaserCompton;
}
