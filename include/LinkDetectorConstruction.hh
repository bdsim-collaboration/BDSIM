#ifndef COUPLINGDETECTORCONSTRUCTION_H
#define COUPLINGDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class DetectorConstruction : G4VUserDetectorConstruction
{
public:
  //! Default constructor
  DetectorConstruction(/* something. special GMAD syntax?*/);

  virtual ~BDSDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();

  
protected:
private:

};

// } // namespace coupling

#endif /* COUPLINGDETECTORCONSTRUCTION_H */
