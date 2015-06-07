#ifndef BDSGEOMETRYCOMPONENT_H
#define BDSGEOMETRYCOMPONENT_H

#include "globals.hh"           // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>              //for std::pair
#include <vector>

/**
 * @brief A generic geometry component for a bdsim model
 * 
 * This class contains the minimum information required
 * for any geometrical object for a bdsim geant4 model
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSGeometryComponent
{
public:
  BDSGeometryComponent(G4VSolid*        containerSolidIn,
		       G4LogicalVolume* containerLVIn);
  BDSGeometryComponent(G4VSolid*        containerSolidIn,
		       G4LogicalVolume* containerLVIn,
		       std::pair<G4double,G4double> extentXIn,
		       std::pair<G4double,G4double> extentYIn,
		       std::pair<G4double,G4double> extentZIn,
		       G4ThreeVector                placementOffset = G4ThreeVector(0,0,0));
  ~BDSGeometryComponent();

  /// Get the name from the container logical volume
  G4String          GetName() const;
  
  /// Get the solid of the container for possible subtraction
  G4VSolid*         GetContainerSolid() const;
  
  /// Get the logical volume of the container for placement
  G4LogicalVolume*  GetContainerLogicalVolume() const;

  /// Get the offset from 0,0,0 that the object should ideally be placed in its parent
  G4ThreeVector     GetPlacementOffset() const;

  /// Set the offset from 0,0,0 that the object should ideally be placed in its parent
  void SetPlacementOffset(G4ThreeVector& offsetIn);
  
  std::pair<G4double,G4double> GetExtentX() const;   ///< get -ve/+ve extent in local x
  std::pair<G4double,G4double> GetExtentY() const;   ///< get -ve/+ve extent in local y
  std::pair<G4double,G4double> GetExtentZ() const;   ///< get -ve/+ve extent in local z
  
  /// Get the extent of the object in the positive direction in all dimensions
  G4ThreeVector GetExtentPositive() const;

  /// Get the extent of the object in the negative direction in all dimensions
  G4ThreeVector GetExtentNegative() const;
  
  void SetExtentX(G4double lowerX, G4double upperX); ///< set the extent in local x
  void SetExtentY(G4double lowerY, G4double upperY); ///< set the extent in local y
  void SetExtentZ(G4double lowerZ, G4double upperZ); ///< set the extent in local z
  void SetExtentX(std::pair<G4double, G4double> extentXIn); ///< set the extent in local x
  void SetExtentY(std::pair<G4double, G4double> extentYIn); ///< set the extent in local y
  void SetExtentZ(std::pair<G4double, G4double> extentZIn); ///< set the extent in local z

  /// Register a logical volume as belonging to this geometry component, which then becomes
  /// responsible for it
  void RegisterLogicalVolume(G4LogicalVolume* logicalVolume);

  /// Apply RegisterLogicalVolume(G4LogicalVolume* logicalVolume) to a vector of logical volumes
  void RegisterLogicalVolumes(std::vector<G4LogicalVolume*> logicalVolumes);

  /// Mark a volume as one that should be made sensitive using the read out geometry.  Note, if
  /// a volume is already sensitive with a specialised sensitive detector, it should NOT be
  /// registered using this method.
  void RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume);

  /// Apply RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume) to a vector of logical volumes
  void RegisterSensitiveVolumes(std::vector<G4LogicalVolume*> sensitiveVolumes);

  /// Access all logical volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;
  
  /// Access all sensitive volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

protected:
  /// Copy constructor
  BDSGeometryComponent(BDSGeometryComponent& component);
  
  G4VSolid*                 containerSolid;
  G4LogicalVolume*          containerLogicalVolume;
  std::pair<G4double, G4double> extentX;  //local -ve,+ve
  std::pair<G4double, G4double> extentY;
  std::pair<G4double, G4double> extentZ;
  std::vector<G4LogicalVolume*> allLogicalVolumes;
  // we have to keep a registry of all logical volumes to be able to associate
  // information with them at construction time - for example S position - that
  // can't be stored in the Logical Volume class itself without modifying geant
  
  /// registry of all volumes that should be made sensitive
  std::vector<G4LogicalVolume*> allSensitiveVolumes;
  G4ThreeVector                 placementOffset;
};

inline G4String  BDSGeometryComponent::GetName() const
{return containerLogicalVolume->GetName();}

inline G4VSolid* BDSGeometryComponent::GetContainerSolid() const
{return containerSolid;}

inline G4LogicalVolume* BDSGeometryComponent::GetContainerLogicalVolume() const
{return containerLogicalVolume;}

inline G4ThreeVector BDSGeometryComponent::GetPlacementOffset() const
{return placementOffset;}

inline void BDSGeometryComponent::SetPlacementOffset(G4ThreeVector& offsetIn)
{placementOffset = G4ThreeVector(offsetIn);}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentX() const
{return extentX;}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentY() const
{return extentY;}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentZ() const
{return extentZ;}

inline G4ThreeVector BDSGeometryComponent::GetExtentPositive() const
{return G4ThreeVector(extentX.second, extentY.second, extentZ.second);}

inline G4ThreeVector BDSGeometryComponent::GetExtentNegative() const
{return G4ThreeVector(extentX.first, extentY.first, extentZ.first);}

inline void BDSGeometryComponent::SetExtentX(G4double lowerX, G4double upperX)
{extentX = std::make_pair(lowerX,upperX);}

inline void BDSGeometryComponent::SetExtentY(G4double lowerY, G4double upperY)
{extentY = std::make_pair(lowerY,upperY);}

inline void BDSGeometryComponent::SetExtentZ(G4double lowerZ, G4double upperZ)
{extentZ = std::make_pair(lowerZ,upperZ);}

inline  void BDSGeometryComponent::SetExtentX(std::pair<G4double, G4double> extentXIn)
{extentX = extentXIn;}

inline  void BDSGeometryComponent::SetExtentY(std::pair<G4double, G4double> extentYIn)
{extentY = extentYIn;}

inline  void BDSGeometryComponent::SetExtentZ(std::pair<G4double, G4double> extentZIn)
{extentZ = extentZIn;}

inline std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllLogicalVolumes() const
{return allLogicalVolumes;}

inline std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllSensitiveVolumes() const
{return allSensitiveVolumes;}

#endif
