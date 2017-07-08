#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"
#include "BDSCavityElement.hh"
#include "BDSCavityFactory.hh"
#include "BDSCavityInfo.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"


BDSCavityElement::BDSCavityElement(G4String             name,
				   G4double             chordLength,
				   G4Material*          vacuumMaterialIn,
				   const BDSFieldInfo*  vacuumFieldIn,
				   const BDSCavityInfo* cavityInfoIn):
  BDSAcceleratorComponent(name, chordLength, 0,
			  "cavity_"+cavityInfoIn->cavityType.ToString()),
  vacuumField(vacuumFieldIn),
  cavityInfo(cavityInfoIn),
  vacuumMaterial(vacuumMaterialIn)
{;}

BDSCavityElement::~BDSCavityElement()
{;}

void BDSCavityElement::Build()
{
  BDSAcceleratorComponent::Build();
  BuildField();
}

void BDSCavityElement::BuildContainerLogicalVolume()
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
}

void BDSCavityElement::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumField,
							    GetAcceleratorVacuumLogicalVolume(),
							    true);
}
