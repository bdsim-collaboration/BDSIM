#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"

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
                G4int      numberWedges     = 3,
                G4double   wedgeBasewidth   = 10,
                G4double   wedgeHeight      = 40,
                G4double   degraderHeight   = 100,
                G4double   materialThickness= 200);
    ~BDSDegrader();
    

protected:
    void Build();
    
    void BuildContainerLogicalVolume();
    
    G4double outerDiameter;
    G4String degraderMaterial;

    //G4VSolid* fullwedge;
    //G4VSolid* halfWedge;
    
};

#endif
