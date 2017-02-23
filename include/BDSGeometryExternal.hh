#ifndef BDSGEOMETRYEXTERNAL_H
#define BDSGEOMETRYEXTERNAL_H

#include "BDSGeometryComponent.hh"

/**
 * @brief A loaded piece of externally provided geometry.
 *
 * This is a wrapper class for BDSGeometryComponent. Currently,
 * this provides no additional members, but experience with beam pipes
 * and magnet outer geometry says that this class will undoubtedly be
 * required. Easier to implement now than rewrite all factories later.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryExternal: public BDSGeometryComponent
{
public:
  /// Use constructors of BDSGeometryComponent
  using BDSGeometryComponent::BDSGeometryComponent;

  virtual ~BDSGeometryExternal(){;}

private:
  /// No default constructor
  BDSGeometryExternal() = delete;
};

#endif
