/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 

#include "BDSLaserWire.hh"
#include "BDSMaterials.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

//============================================================

BDSLaserWire::BDSLaserWire (G4String aName,G4double aLength,
G4double aWavelength, G4ThreeVector aDirection):
  BDSAcceleratorComponent(
    aName,
    aLength,0,0,0),
  itsLaserCompton(NULL),
  itsLaserDirection(aDirection),itsLaserWavelength(aWavelength)
{
}

void BDSLaserWire::BuildMarkerLogicalVolume()
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
  itsMarkerLogicalVolume->SetVisAttributes(itsVisAttributes);
  BDSGlobalConstants::Instance()->
    SetLaserwireWavelength(itsMarkerLogicalVolume->GetName(),itsLaserWavelength);
  BDSGlobalConstants::Instance()->
    SetLaserwireDir(itsMarkerLogicalVolume->GetName(),itsLaserDirection);
}

void BDSLaserWire::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.,1.,0.));
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
}

BDSLaserWire::~BDSLaserWire()
{
  delete itsLaserCompton;
}
