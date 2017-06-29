#include "BDSAcceleratorComponent.hh"
#include "BDSCavityElement.hh"
#include "BDSCavityFactory.hh"
#include "BDSCavityInfo.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"


BDSCavityElement::BDSCavityElement(G4String             name,
				   G4double             length,
				   G4Material*          vacuumMaterialIn,
				   const BDSFieldInfo*  vacuumFieldIn,
				   const BDSCavityInfo* cavityInfoIn):
  BDSAcceleratorComponent(name, length, 0,
			  "cavity_"+cavityInfoIn->cavityType.ToString()),
  vacuumField(vacuumFieldIn),
  cavityInfo(cavityInfoIn),
  vacuumMaterial(vacuumMaterialIn),
  vacuumLV(nullptr)
{
  /*
  cavityInfo   = vacuumField->CavityInfo(); // create shortcut for convenience
  cavityRadius = cavityInfo->equatorRadius;
  thickness    = cavityInfo->thickness;
  irisRadius   = cavityInfo->irisRadius;

  if (cavityRadius < (irisRadius + thickness))
    {
      G4cout << "Invalid geometry for \"" << name << "\" - increase equatorRadius" << G4endl;
      exit(1);
    }
  */
}

BDSCavityElement::~BDSCavityElement()
{
  //delete vacuumField;
}

void BDSCavityElement::Build()
{
  BDSCavity* cavity = BDSCavityFactory::Instance()->CreateCavity(name, cavityInfo);
  //vacuumLV = cav
  BDSAcceleratorComponent::Build();
  BuildField();
}

void BDSCavityElement::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumField,
							    vacuumLV,
							    true);
}
/*
void BDSCavityElement::PlaceComponents()
{
  G4PVPlacement* vacuumPV = new G4PVPlacement(nullptr,               //Rotation
					      G4ThreeVector(0,0,0),  //Position
					      vacuumLV,              //Logical Volume to be place
					      name + "_vacuum_pv",   //placement name
					      containerLogicalVolume,//mother volume
					      false,                 //pMany unused
					      0,                     //copy number
					      checkOverlaps);        //check overlaps
  
  RegisterPhysicalVolume(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement(nullptr,               //Rotation
					      G4ThreeVector(0,0,0),  //Position
					      cavityLV,              //Logical Volume to be place
					      name + "_cavity_pv",   //placement name
					      containerLogicalVolume,//mother volume
					      false,                 //pMany unused
					      0,                     //copy number
					      checkOverlaps);        //check overlaps
  
  RegisterPhysicalVolume(cavityPV); 
}

void BDSCavityElement::BuildContainerLogicalVolume() 
{
  G4double outerRadius = cavityRadius + thickness + lengthSafety;
  containerSolid = new G4Tubs(name + "_container_solid",   //name
			      0.0,                         //innerRadius
			      outerRadius,                 //outerRadius
			      chordLength*0.5,             //half length
			      0.0,                         //starting angle
			      2.0*CLHEP::pi);              //spanning angle

  SetExtent(BDSExtent(outerRadius, outerRadius,  chordLength*0.5));
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}
*/
