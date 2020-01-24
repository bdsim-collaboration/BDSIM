#ifndef BDSLINKDETECTORCONSTRUCTION_H
#define BDSLINKDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class BDSLinkDetectorConstruction : G4VUserDetectorConstruction
{
public:
  //! Default constructor
  DetectorConstruction(/* something. special GMAD syntax?*/);

  virtual ~BDSDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();

  
protected:
private:

};

// } // namespace BDSLINK

#endif /* BDSLINKDETECTORCONSTRUCTION_H */
