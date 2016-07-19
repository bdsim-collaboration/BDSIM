#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuter.hh"
#include "BDSSimpleComponent.hh"

class BDSExtent;
class G4VSolid;
class G4LogicalVolume;

BDSMagnetOuter::BDSMagnetOuter(G4VSolid*                    containerSolid,
			       G4LogicalVolume*             containerLV,
			       BDSExtent                    extent,
			       BDSGeometryComponent*        magnetContainerIn,
			       G4ThreeVector                placementOffset,
			       BDSSimpleComponent*          endPieceBeforeIn,
			       BDSSimpleComponent*          endPieceAfterIn):
  BDSGeometryComponent(containerSolid, containerLV, extent, BDSExtent(),
		       placementOffset),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn):
  BDSGeometryComponent(*componentIn),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn)
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
  if (endPieceBefore)
    {delete endPieceBefore; endPieceBefore = nullptr;}
  //if (endPieceAfter)
  //  {delete endPieceAfter; endPieceAfter = nullptr;}
}

BDSMagnetOuter::~BDSMagnetOuter()
{
  ClearMagnetContainer();
  ClearEndPieces();
}
