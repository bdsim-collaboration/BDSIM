#ifndef BDSGEOMETRYEXTERNAL_H
#define BDSGEOMETRYEXTERNAL_H

#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"

#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VSolid;
namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief A loaded piece of externally provided geometry.
 *
 * This is a wrapper class for BDSGeometryComponent.  Currently, 
 * this provides no additional members, but experience with beam pipes
 * and magnet outer geometry says that this class will undoubtedly be
 * required.  Easier to implement now that rewrite all factories later.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryExternal: public BDSGeometryComponent
{
public:
  /// Constructor to mirror that of BDSGeometryComponent
  BDSGeometryExternal(G4VSolid*        containerSolidIn,
		      G4LogicalVolume* containerLVIn);
  /// Constructor to mirror that of BDSGeometryComponent
  BDSGeometryExternal(G4VSolid*         containerSolidIn,
		      G4LogicalVolume*  containerLVIn,
		      BDSExtent         extentIn,
		      BDSExtent         innerExtentIn       = BDSExtent(),
		      G4ThreeVector     placementOffsetIn   = G4ThreeVector(0,0,0),
		      G4RotationMatrix* placementRotationIn = nullptr);
  
  virtual ~BDSGeometryExternal();

private:
  /// No default constructor
  BDSGeometryExternal() = delete;

};

#endif
