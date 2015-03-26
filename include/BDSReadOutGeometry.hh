#ifndef BDSREADOUTGEOMETRY_H
#define BDSREADOUTGEOMETRY_H

#include "globals.hh"             // geant4 globals / types
#include "G4VReadOutGeometry.hh"
#include "G4VPhysicalVolume.hh"

/**
 * @brief Class that generates readout geometry for enegy
 * counter sensitive detectors. A la Geant4, this would actually
 * construct a 'parallel' (not to be confused with the G4ParallelWorld
 * class, which is unrelated) world separately and after the main
 * 'mass' world.  However, this is really quite inconvenient so both
 * are constructed at the same time from the same accelerator components
 * and this is a dummy class required by Geant4 that simply gets the 
 * already constructed world and returns it.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSReadOutGeometry: public G4VReadOutGeometry {
 public:
  BDSReadOutGeometry(G4String name);
  ~BDSReadOutGeometry();

private:
  G4VPhysicalVolume* Build();

};

#endif
