#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDegrader.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4ExtrudedSolid.hh"



#include "globals.hh" // geant4 globals / types

BDSDegrader::BDSDegrader (G4String   name, 
                    G4double   length,
                    G4double   outerDiameter,
                    G4String   degraderMaterial,
                    G4int      numberWedges,
                    G4double   wedgeBasewidth,
                    G4double   wedgeHeight,
                    G4double   degraderHeight,
                    G4double   materialThickness):
BDSAcceleratorComponent(name, length, 0, "degrader")
{;}

BDSDegrader::~BDSDegrader()
{;}

void BDSDegrader::BuildContainerLogicalVolume()
{
        //WRONG G4Box* containerSolid = 
    // containerSolid = new G4Boxsjfdlkdsjfl
    // build solid - G4Box or G4Tubs - called containerSolid
    // build logical volume - use BDSGlobalConstants::Instance()->GetEmptyMaterial()
    // should be called containerLV

}

void BDSDegrader::Build()
{  
    
    // solid
    // logical volume
    // vis attributes
    // user limits
    // place logical volume in containerLV

}
