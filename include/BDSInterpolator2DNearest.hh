#ifndef BDSINTERPOLATOR2DNEAREST_H
#define BDSINTERPOLATOR2DNEAREST_H

#include "BDSArray2DCoords.hh"
#include "BDSInterpolator2D.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * @author Laurie Nevay
 */

template<typename T>
class BDSInterpolator2DNearest: public BDSInterpolator2D<T>
{
public:
  BDSInterpolator2DNearest(BDSArray2DCoords<T>* arrayIn):
    array(*arrayIn)
  {;}
  ~BDSInterpolator2DNearest();

  /// Copy constructor
  BDSInterpolator2DNearest(const BDSInterpolator2DNearest& other)
  {array(other.array);}

  /// Main accessor this class has to provide. This is relatively simple
  /// so included here.
  virtual T GetInterpolatedValue(G4double x, G4double y) const
  {return array(array.NearestXY(x,y));}
  
private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DNearest();

  /// Unused assignment operator.
  BDSInterpolator2DNearest& operator=(const BDSInterpolator2DNearest& rhs);

  /// The field data.
  BDSArray2DCoords<T> array;
};

#endif
