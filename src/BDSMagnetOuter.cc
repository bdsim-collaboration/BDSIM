#include "BDSGeometryComponent.hh"
#include "BDSGeometryComponentHollow.hh"
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
			       BDSGeometryComponentHollow*  endPieceBeginningIn,
			       BDSGeometryComponentHollow*  endPieceEndIn):
  BDSGeometryComponent(containerSolid, containerLV, extentX, extentY, extentZ,
		       placementOffset),
  magnetContainer(magnetContainerIn),
  endPieceBeginning(endPieceBeginningIn),
  endPieceEnd(endPieceEndIn)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       BDSGeometryComponentHollow* endPieceBeginningIn,
			       BDSGeometryComponentHollow* endPieceEndIn):
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
