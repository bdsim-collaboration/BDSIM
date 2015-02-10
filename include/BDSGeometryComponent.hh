#ifndef BDSGEOMETRYCOMPONENT_H
#define BDSGEOMETRYCOMPONENT_H

#include "globals.hh"           // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>              //for std::pair

/**
 * @brief a generic geometry component for a bdsim model
 * 
 * This class contains the minimum information required
 * for any object to be place in the bdsim model
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
		       std::pair<G4double,G4double> extentZIn);
  ~BDSGeometryComponent();

  G4VSolid*         GetContainerSolid(); /// get the solid of the container for possible subtraction
  G4LogicalVolume*  GetContainerLogicalVolume(); /// get the logical volume of the container for placement
  std::pair<G4double,G4double> GetExtentX(); /// get -ve/+ve extent in local x
  std::pair<G4double,G4double> GetExtentY(); /// get -ve/+ve extent in local y
  std::pair<G4double,G4double> GetExtentZ(); /// get -ve/+ve extent in local z
  void SetExtentX(G4double lowerX, G4double upperX); /// set the extent in local x
  void SetExtentY(G4double lowerY, G4double upperY); /// set the extent in local y
  void SetExtentZ(G4double lowerZ, G4double upperZ); /// set the extent in local z

protected:
  G4VSolid*                 containerSolid;
  G4LogicalVolume*          containerLogicalVolume;
  std::pair<G4double, G4double> extentX;  //local -ve,+ve
  std::pair<G4double, G4double> extentY;
  std::pair<G4double, G4double> extentZ;
};

inline G4VSolid* BDSGeometryComponent::GetContainerSolid()
{return containerSolid;}

inline G4LogicalVolume* BDSGeometryComponent::GetContainerLogicalVolume()
{return containerLogicalVolume;}

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

#endif
