#ifndef BDSBEAMPIPE_H
#define BDSBEAMPIPE_H

#include "BDSGeometryComponent.hh"

#include "globals.hh"               // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include <utility>                  // for std::pair

class BDSExtent;

/**
 * @brief A holder class for all information required for a
 * piece of beampipe.  
 * 
 * This does not implement the construction of the beampipe but merely
 * holds all relevant objects and information. 
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipe: public BDSGeometryComponent
{
public:
  /// constructor has BDSGeometryComponent members first,
  /// then everything extra for this derived class
  BDSBeamPipe(G4VSolid*                 containerSolidIn,
	      G4LogicalVolume*          containerLVIn,
	      BDSExtent                 extentIn,
	      G4VSolid*                 containerSubtractionSolidIn,
	      G4LogicalVolume*          vacuumLVIn,
	      G4bool                    containerIsCircularIn = false,
	      G4double                  containerRadiusIn     = 0.0,
	      G4ThreeVector             inputFaceNormalIn  = G4ThreeVector(0,0,-1),
	      G4ThreeVector             outputFaceNormalIn = G4ThreeVector(0,0, 1));
  
  virtual ~BDSBeamPipe(); /// default destructor sufficient as G4 manages solids and LVs

  /// Access a solid for beampipe subtraction - note this is typically longer
  /// than the actual beampipe for unambiguous subtraction
  G4VSolid*        GetContainerSubtractionSolid() const;
  /// Access the vacuum volume to set fields and limits
  G4LogicalVolume* GetVacuumLogicalVolume() const;
  /// Flag to tell whether the parent volume needn't use a subtraction
  /// solid and can simply use a G4Tubs for example
  G4bool           ContainerIsCircular() const;
  /// If it is circular, we need the radius
  G4double         GetContainerRadius() const;
  
protected:
  G4VSolid*        containerSubtractionSolid;
  G4LogicalVolume* vacuumLogicalVolume;
  G4bool           containerIsCircular;
  G4double         containerRadius;
  G4ThreeVector    inputFaceNormal;
  G4ThreeVector    outputFaceNormal;
};

inline G4VSolid* BDSBeamPipe::GetContainerSubtractionSolid() const
{return containerSubtractionSolid;}

inline G4LogicalVolume* BDSBeamPipe::GetVacuumLogicalVolume() const
{return vacuumLogicalVolume;}

inline G4bool BDSBeamPipe::ContainerIsCircular() const
{return containerIsCircular;}

inline G4double BDSBeamPipe::GetContainerRadius() const
{return containerRadius;}

#endif
