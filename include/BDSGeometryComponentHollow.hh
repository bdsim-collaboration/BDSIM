#ifndef BDSGEOMETRYCOMPONENTHOLLOW_H
#define BDSGEOMETRYCOMPONENTHOLLOW_H

#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include <utility>

/**
 * @brief Extension of BDSGeometryComponent for hollow component.
 * 
 * This class provides interior extents as well as the exterior ones
 * provided by BDSGeometryComponent. This allows two independent items
 * to be placed around each other and know if they'll (roughly) overlap
 * without knowing their exact shape (square, triangle, circle etc).
 *
 * @author Laurie Nevay
 */

class BDSGeometryComponentHollow: public BDSGeometryComponent
{
public:
  BDSGeometryComponentHollow(G4VSolid*        containerSolidIn,
			     G4LogicalVolume* containerLVIn);
  BDSGeometryComponentHollow(G4VSolid*        containerSolidIn,
			     G4LogicalVolume* containerLVIn,
			     std::pair<G4double,G4double> extentXIn,
			     std::pair<G4double,G4double> extentYIn,
			     std::pair<G4double,G4double> extentZIn,
			     std::pair<G4double,G4double> innerExtentXIn,
			     std::pair<G4double,G4double> innerExtentYIn,
			     std::pair<G4double,G4double> innerExtentZIn,
			     G4ThreeVector                placementOffset = G4ThreeVector(0,0,0));

  virtual ~BDSGeometryComponentHollow() {;}

  /// @{ Get -ve/+ve inner extent in local coords.
  std::pair<G4double,G4double> GetExtentX() const {return innerExtentX;}   
  std::pair<G4double,G4double> GetExtentY() const {return innerExtentY;}
  std::pair<G4double,G4double> GetExtentZ() const {return innerExtentZ;}
  /// @}

  /// @{ Set the -ve/+ve inner extent in local coords.
  void SetInnerExtentX(G4double lowerX, G4double upperX);
  void SetInnerExtentY(G4double lowerY, G4double upperY);
  void SetInnerExtentZ(G4double lowerZ, G4double upperZ);
  void SetInnerExtentX(std::pair<G4double, G4double> extentXIn);
  void SetInnerExtentY(std::pair<G4double, G4double> extentYIn);
  void SetInnerExtentZ(std::pair<G4double, G4double> extentZIn);
  /// @}

private:
  std::pair<G4double, G4double> innerExtentX;
  std::pair<G4double, G4double> innerExtentY;
  std::pair<G4double, G4double> innerExtentZ;

};

inline void BDSGeometryComponentHollow::SetInnerExtentX(G4double lowerX, G4double upperX)
{innerExtentX = std::make_pair(lowerX,upperX);}

inline void BDSGeometryComponentHollow::SetInnerExtentY(G4double lowerY, G4double upperY)
{innerExtentY = std::make_pair(lowerY,upperY);}

inline void BDSGeometryComponentHollow::SetInnerExtentZ(G4double lowerZ, G4double upperZ)
{innerExtentZ = std::make_pair(lowerZ,upperZ);}

inline  void BDSGeometryComponentHollow::SetInnerExtentX(std::pair<G4double, G4double> extentXIn)
{innerExtentX = extentXIn;}

inline  void BDSGeometryComponentHollow::SetInnerExtentY(std::pair<G4double, G4double> extentYIn)
{innerExtentY = extentYIn;}

inline  void BDSGeometryComponentHollow::SetInnerExtentZ(std::pair<G4double, G4double> extentZIn)
{innerExtentZ = extentZIn;}

#endif
