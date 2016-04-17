#ifndef BDSINTERPOLATOR2DNEAREST_H
#define BDSINTERPOLATOR2DNEAREST_H

#include "BDSArray2DCoord.hh"

template<typename T>
class BDSInterpolator2DNearest
{
public:
  BDSInterpolator2DNearest(BDSArray2DCoords<T> arrayIn);
  ~BDSInterpolator2DNearest();

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DNearest();

  /// The field data.
  BDSArray2DCoords<T> array;

};

#endif
