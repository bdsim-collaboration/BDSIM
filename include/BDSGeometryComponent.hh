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
 * for any object to be placed in the bdsim model
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
  ~BDSGeometryComponent(){;};  ///> Does not delete anything contained in pointers as G4 does that

  G4VSolid*         GetContainerSolid(); ///> get the solid of the container for possible subtraction
  G4LogicalVolume*  GetContainerLogicalVolume(); ///> get the logical volume of the container for placement
  G4ThreeVector     GetPlacementOffset();///> get the offset from 0,0,0 that the object should ideally be placed in its parent
  void SetPlacementOffset(G4ThreeVector& offsetIn);///> set the offset from 0,0,0 that the object should ideally be placed in its parent
  std::pair<G4double,G4double> GetExtentX(); ///> get -ve/+ve extent in local x
  std::pair<G4double,G4double> GetExtentY(); ///> get -ve/+ve extent in local y
  std::pair<G4double,G4double> GetExtentZ(); ///> get -ve/+ve extent in local z
  void SetExtentX(G4double lowerX, G4double upperX); ///> set the extent in local x
  void SetExtentY(G4double lowerY, G4double upperY); ///> set the extent in local y
  void SetExtentZ(G4double lowerZ, G4double upperZ); ///> set the extent in local z
  void SetExtentX(std::pair<G4double, G4double> extentXIn);
  void SetExtentY(std::pair<G4double, G4double> extentYIn);
  void SetExtentZ(std::pair<G4double, G4double> extentZIn);
  void RegisterLogicalVolume(G4LogicalVolume* logicalVolume);
  void RegisterLogicalVolumes(std::vector<G4LogicalVolume*> logicalVolumes);
  void RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume);
  void RegisterSensitiveVolumes(std::vector<G4LogicalVolume*> sensitiveVolumes);
  std::vector<G4LogicalVolume*> GetAllSensitiveVolumes();
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes();

protected:
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
  // can't be stored in the Logical Volume class itself without modifying Geant

private:
  /// Private default constructor - force use of specified constructors
  BDSGeometryComponent();
};

inline G4VSolid* BDSGeometryComponent::GetContainerSolid()
{return containerSolid;}

inline G4LogicalVolume* BDSGeometryComponent::GetContainerLogicalVolume()
{return containerLogicalVolume;}

inline G4ThreeVector BDSGeometryComponent::GetPlacementOffset()
{return placementOffset;}

inline void BDSGeometryComponent::SetPlacementOffset(G4ThreeVector& offsetIn)
{placementOffset = G4ThreeVector(offsetIn);}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentX()
{return extentX;}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentY()
{return extentY;}

inline std::pair<G4double,G4double> BDSGeometryComponent::GetExtentZ()
{return extentZ;}

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

inline void BDSGeometryComponent::RegisterLogicalVolume(G4LogicalVolume* logicalVolume)
{allLogicalVolumes.push_back(logicalVolume);}

inline void BDSGeometryComponent::RegisterLogicalVolumes(std::vector<G4LogicalVolume*> logicalVolumes)
{allLogicalVolumes.insert(allLogicalVolumes.end(), logicalVolumes.begin(), logicalVolumes.end());}

inline std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllLogicalVolumes()
{return allLogicalVolumes;}

inline void BDSGeometryComponent::RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume)
{allSensitiveVolumes.push_back(sensitiveVolume);}

inline void BDSGeometryComponent::RegisterSensitiveVolumes(std::vector<G4LogicalVolume*> sensitiveVolumes)
{allSensitiveVolumes.insert(allSensitiveVolumes.end(), sensitiveVolumes.begin(), sensitiveVolumes.end());}

inline std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllSensitiveVolumes()
{return allSensitiveVolumes;}

#endif
