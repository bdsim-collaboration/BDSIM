#include "BDSBeamPipeInfo.hh"
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
  itsLaserDirection(direction),
  itsLaserWavelength(wavelength)
{;}

void BDSLaserWire::BuildContainerLogicalVolume()
{
  G4double beamPipeRadius = BDSGlobalConstants::Instance()->GetDefaultBeamPipeModel()->aper1;
  containerSolid = new G4Box(name +"_container_solid",
			     beamPipeRadius,
			     beamPipeRadius,
			     chordLength*0.5);
  containerLogicalVolume=new G4LogicalVolume(containerSolid,
					     BDSMaterials::Instance()->GetMaterial("LaserVac"),
					     name + "_container_lv");
  BDSGlobalConstants::Instance()->SetLaserwireWavelength(containerLogicalVolume->GetName(),itsLaserWavelength);
  BDSGlobalConstants::Instance()->SetLaserwireDir(containerLogicalVolume->GetName(),itsLaserDirection);

  SetAcceleratorVacuumLogicalVolume(containerLogicalVolume);
}

BDSLaserWire::~BDSLaserWire()
{}
