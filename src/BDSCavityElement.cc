#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"
#include "BDSCavityElement.hh"
#include "BDSCavityFactory.hh"
#include "BDSCavityInfo.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"


BDSCavityElement::BDSCavityElement(G4String             nameIn,
				   G4double             chordLengthIn,
				   G4Material*          vacuumMaterialIn,
				   const BDSFieldInfo*  vacuumFieldInfoIn,
				   const BDSCavityInfo* cavityInfoIn):
  BDSAcceleratorComponent(nameIn, chordLengthIn, 0,
			  "cavity_"+cavityInfoIn->cavityType.ToString()),
  vacuumFieldInfo(vacuumFieldInfoIn),
  cavityInfo(cavityInfoIn),
  vacuumMaterial(vacuumMaterialIn)
{;}

BDSCavityElement::~BDSCavityElement()
{
  delete vacuumFieldInfo;
  delete cavityInfo;
}

void BDSCavityElement::BuildContainerLogicalVolume()
{;}

void BDSCavityElement::Build()
{
  BDSCavity* cavity = BDSCavityFactory::Instance()->CreateCavity(name,
								 chordLength,
								 cavityInfo,
								 vacuumMaterial);
  RegisterDaughter(cavity);

  // make the beam pipe container, this object's container
  containerLogicalVolume = cavity->GetContainerLogicalVolume();
  containerSolid         = cavity->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(cavity->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(cavity);

  // field
  BuildField();
}

void BDSCavityElement::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
							    GetAcceleratorVacuumLogicalVolume(),
							    true);
}
