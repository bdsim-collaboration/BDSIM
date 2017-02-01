#ifndef BDSDEGRADER_H
#define BDSDEGRADER_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Degrader based on wedge design used in the PSI medical accelerator.
 * 
 * @author Will Shields
 */

class BDSDegrader: public BDSAcceleratorComponent
{
public:
  BDSDegrader(G4String name, 
	      G4double   length,
	      G4double   outerDiameter,
	      G4int      numberWedges,
	      G4double   wedgeLength,
	      G4double   degraderHeight,
	      G4double   degraderOffset,
	      G4String   degraderMaterial     = "carbon");
  virtual ~BDSDegrader();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();
  
  G4double outerDiameter;
  G4int    numberWedges;
  G4double wedgeLength;
  G4double degraderHeight;
  G4double degraderOffset;
  G4String degraderMaterial;
  
  
  bool isOdd(G4int integer)
  {
    if (integer % 2 != 0)
      return true;
    else
      return false;
  }
  
  bool isEven(G4int integer)
  {
    if (integer % 2 == 0)
      return true;
    else
      return false;
  }
};

#endif
