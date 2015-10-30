#ifndef BDSCOLLIMATORELLIPTICAL_H
#define BDSCOLLIMATORELLIPTICAL_H

#include "BDSCollimatorBase.hh"

#include "globals.hh" // geant4 globals / types

class BDSCollimatorElliptical: public BDSCollimatorBase
{
public:
  BDSCollimatorElliptical(G4String name,
			  G4double length,
			  G4double outerDiameter,
			  G4double xApertureIn        = 0,
			  G4double yApertureIn        = 0,
			  G4double xOutApertureIn     = 0,
			  G4double yOutApertureIn     = 0,
			  G4String collimatorMaterial = "copper",
			  G4String vacuumMaterial     = "vacuum");
  ~BDSCollimatorElliptical(){;};

  virtual void BuildInnerCollimator();
};

#endif
