#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class G4VSolid;

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. More factorised geometry construction will mean
 * SDs will be assigned in construction and not in detector
 * construction afterwards as a post processing step. This 
 * instantiates all necessary SDs and holds them.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
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
    
    G4VSolid* fullwedge;
    G4VSolid* halfWedge;
    
};

#endif
