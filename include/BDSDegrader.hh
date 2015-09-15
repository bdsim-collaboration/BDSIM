#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "BDSAcceleratorComponent.hh"

class G4VSolid;

/**
 * @brief A class for a degrader.
 * 
 * @author Will Shields <william.shields.2010@live.rhul.ac.uk>
 */

class BDSDegrader: public BDSAcceleratorComponent
{
public:
    BDSDegrader(G4String name, 
                G4double   length,
                G4double   outerDiameter,
                G4String   degraderMaterial,
                G4int      numberWedges     = 5,
                G4double   wedgeBasewidth   = 10,
                G4double   wedgeHeight      = 100,
                G4double   degraderHeight   = 100,
                G4double   materialThickness= 200);
    ~BDSDegrader();
    

protected:
    void Build();
    
    void BuildContainerLogicalVolume();
    
    G4double outerDiameter;
    G4String degraderMaterial;
    G4int numberWedges;
    G4double wedgeBasewidth;
    G4double wedgeHeight;
    G4double degraderHeight;
    G4double materialThickness;

    //G4VSolid* fullWedge;
    //G4VSolid* halfWedge;
    //G4VSolid* base;

    
};

#endif
