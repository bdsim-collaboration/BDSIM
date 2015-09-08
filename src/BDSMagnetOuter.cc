#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuter.hh"

#include <utility>

class G4VSolid;
class G4LogicalVolume;

BDSMagnetOuter::BDSMagnetOuter(G4VSolid*                    containerSolid,
			       G4LogicalVolume*             containerLV,
			       std::pair<G4double,G4double> extentX,
			       std::pair<G4double,G4double> extentY,
			       std::pair<G4double,G4double> extentZ,
			       BDSGeometryComponent*        magnetContainerIn,
			       G4ThreeVector                placementOffset):
  BDSGeometryComponent(containerSolid, containerLV, extentX, extentY, extentZ,
		       placementOffset),
  magnetContainer(magnetContainerIn)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn):
  BDSGeometryComponent(*componentIn),
  magnetContainer(magnetContainerIn)
{;}

void BDSMagnetOuter::ClearMagnetContainer()
{
  if (magnetContainer)
    {
      delete magnetContainer;
      magnetContainer = nullptr;
    }
}

BDSMagnetOuter::~BDSMagnetOuter()
{
  ClearMagnetContainer();
}
