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

class BDSGeometryExternal: public BDSGeometryComponent
{
public:
  BDSGeometryExternal(G4VSolid*        containerSolidIn,
		      G4LogicalVolume* containerLVIn);
  BDSGeometryExternal(G4VSolid*         containerSolidIn,
		      G4LogicalVolume*  containerLVIn,
		      BDSExtent         extentIn,
		      BDSExtent         innerExtentIn       = BDSExtent(),
		      G4ThreeVector     placementOffsetIn   = G4ThreeVector(0,0,0),
		      G4RotationMatrix* placementRotationIn = nullptr);
  
  virtual ~BDSGeometryExternal();

private:
  BDSGeometryExternal() = delete;

};

#endif
