#ifndef BDSCOLLIMATORRECTANGULAR_H
#define BDSCOLLIMATORRECTANGULAR_H

#include "BDSCollimatorBase.hh"

/**
 * @brief A class for a rectangular collimator.
 */

class BDSCollimatorRectangular: public BDSCollimatorBase
{
public:
  BDSCollimatorRectangular(G4String name,
			   G4double length,
			   G4double outerDiameter,
			   G4double xApertureIn        = 0,
			   G4double yApertureIn        = 0,
			   G4double xOutApertureIn     = 0,
			   G4double yOutApertureIn     = 0,
			   G4String collimatorMaterial = "copper",
			   G4String vacuumMaterial     = "vacuum",
			   G4String colour             = "collimator");
  virtual ~BDSCollimatorRectangular(){;};

  virtual void BuildInnerCollimator();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorRectangular();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorRectangular& operator=(const BDSCollimatorRectangular&) = delete;
  BDSCollimatorRectangular(BDSCollimatorRectangular&) = delete;
  ///@}
};

#endif
