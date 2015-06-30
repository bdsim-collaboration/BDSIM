/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
#include "BDSLaserWire.hh"
#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

BDSLaserWire::BDSLaserWire(G4String      name,
			   G4double      length,
			   G4double      wavelength,
			   G4ThreeVector direction):
  BDSAcceleratorComponent(name, length, 0, "laserwire"),
  itsLaserCompton(NULL),
  itsLaserDirection(direction),
  itsLaserWavelength(wavelength)
{;}

void BDSLaserWire::BuildContainerLogicalVolume()
{
  G4double beamPipeRadius = BDSGlobalConstants::Instance()->GetBeamPipeRadius();
  containerSolid = new G4Box(name +"_container_solid",
			     beamPipeRadius,
			     beamPipeRadius,
			     chordLength*0.5);
  containerLogicalVolume=new G4LogicalVolume(containerSolid,
					     BDSMaterials::Instance()->GetMaterial("LaserVac"),
					     name + "_container_lv");
  BDSGlobalConstants::Instance()->
    SetLaserwireWavelength(containerLogicalVolume->GetName(),itsLaserWavelength);
  BDSGlobalConstants::Instance()->
    SetLaserwireDir(containerLogicalVolume->GetName(),itsLaserDirection);
}

BDSLaserWire::~BDSLaserWire()
{
  delete itsLaserCompton;
}
