#ifndef BDSINTERPOLATE2DLINEAR_H
#define BDSINTERPOLATE2DLINEAR_H

#include "BDSArray2DCoords.hh"
#include "BDSInterpolator2D.hh"

#include "globals.hh" // geant4 types / globals

#include <cmath>

/** 
 * @brief Linear interpolation over 2d array.
 * 
 * This uses a bilinear algorithm to interpolate a 2d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops abruptly to 0 outside the specified region.
 * 
 * @author Laurie Nevay
 */

template<typename T>
class BDSInterpolator2DLinear: public BDSInterpolator2D<T>
{
public:
  BDSInterpolator2DLinear(BDSArray2DCoords<T>* arrayIn):
  array(*arrayIn)
  {;}
  ~BDSInterpolator2DLinear();

  virtual T GetInterpolatedValue(G4double x, G4double y) const
  { 
    G4double xarr = array.ArrayCoordsFromX(x);
    G4double yarr = array.ArrayCoordsFromY(y);

    G4double x1 = floor(xarr);
    G4double x2 = ceil(xarr);
    G4double y1 = floor(yarr);
    G4double y2 = ceil(yarr);

    T Q11 = array(x1,y1);
    T Q12 = array(x1,y2);
    T Q22 = array(x2,y2);
    T Q21 = array(x2,y1);

    G4double x2mx = x2 - x;
    G4double y2my = y2 - y;
    G4double xmx1 = x  - x1;
    G4double ymy1 = y  - y1;
    G4double factor = 1./(array.XStep()*array.YStep());
    T result =  (Q11 * x2mx * y2my) + (Q21 * xmx1 * y2my) +
      (Q12 * x2mx * ymy1) + (Q22 * xmx1 * ymy1);

    result *= factor;
    return result;
  }

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DLinear();

  /// The field data
  BDSArray2DCoords<T> array;
};

#endif
