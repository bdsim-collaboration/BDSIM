#ifndef BDSCAVITY_H
#define BDSCAVITY_H

#include "BDSGeometryComponent.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

class BDSExtent;

class G4LogicalVolume;
class G4VSolid;

/**
 * @brief A holder class for an RF cavity piece of geometry.
 * 
 * @author Laurie Nevay
 */

class BDSCavity: public BDSGeometryComponent
{
public:
  BDSCavity(G4VSolid*         containerSolidIn,
	    G4LogicalVolume*  containerLVIn,
	    BDSExtent         extentIn,
	    G4LogicalVolume*  vacuumLVIn,
	    G4ThreeVector     placementOffsetIn   = G4ThreeVector(0,0,0),
	    G4RotationMatrix* placementRotationIn = nullptr);

  virtual ~BDSCavity() {;}

  /// Access the vacuum volume to set fields and limits
  inline G4LogicalVolume* GetVacuumLogicalVolume() const {return vacuumLogicalVolume;};

protected:
  G4LogicalVolume* vacuumLogicalVolume; ///< Cache of which volume is the vacuum one.
};

#endif
