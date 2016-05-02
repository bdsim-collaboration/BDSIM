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
			       G4ThreeVector                placementOffset,
			       BDSGeometryComponent*        endPieceBeginningIn,
			       BDSGeometryComponent*        endPieceEndIn):
  BDSGeometryComponent(containerSolid, containerLV, extentX, extentY, extentZ,
		       placementOffset),
  magnetContainer(magnetContainerIn),
  endPieceBeginning(endPieceBeginningIn),
  endPieceEnd(endPieceEndIn)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       BDSGeometryComponent* endPieceBeginningIn,
			       BDSGeometryComponent* endPieceEndIn):
  BDSGeometryComponent(*componentIn),
  magnetContainer(magnetContainerIn),
  endPieceBeginning(endPieceBeginningIn),
  endPieceEnd(endPieceEndIn)
{;}

void BDSMagnetOuter::ClearMagnetContainer()
{
  if (magnetContainer)
    {
      delete magnetContainer;
      magnetContainer = nullptr;
    }
}

void BDSMagnetOuter::ClearEndPieces()
{
  if (endPieceBeginning)
    {delete endPieceBeginning; endPieceBeginning = nullptr;}
  if (endPieceEnd)
    {delete endPieceEnd; endPieceEnd = nullptr;}
}

BDSMagnetOuter::~BDSMagnetOuter()
{
  ClearMagnetContainer();
  ClearEndPieces();
}
