#include "BDSFieldMagGradient.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"


#include "BDSInterpolatorType.hh"
#include "BDSFieldMag.hh"
#include "BDSMagnetStrength.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"

BDSFieldMagGradient::BDSFieldMagGradient(){

}

BDSMagnetStrength* BDSFieldMagGradient::CalculateMultipoles(BDSFieldMagInterpolated2D* BField,
							      G4int order)
{
/*
  //finds k1 - quadrupoles
  G4double k1 =  1storder()
  {
};
  //finds k2 - sextupoles
  float k2 2ndorder()
  {
};
  //finds k3 - ocutpoles
  float k3 3rdorder()
  {
};
  //finds k4 - decapoles
  float k4 4thorder(){

  }
  //finds k5 - dodecapoles
  float k5 5thorder(){

*/
 BDSMagnetStrength* a = new BDSMagnetStrength();
  return a;

}
