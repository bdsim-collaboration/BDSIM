#ifndef BDSCOLLIMATORRECTANGULAR_H
#define BDSCOLLIMATORRECTANGULAR_H

#include "BDSCollimatorBase.hh"

class BDSCollimatorRectangular: public BDSCollimatorBase
{
public:
  BDSCollimatorRectangular(G4String name,
			   G4double length,
			   G4double outerDiameter,
			   G4double xApertureIn        = 0,
			   G4double yApertureIn        = 0,
			   G4String collimatorMaterial = "copper",
			   G4String vacuumMaterial     = "vacuum");
  ~BDSCollimatorRectangular(){;};

  virtual void BuildInnerCollimator();
  
};

#endif
