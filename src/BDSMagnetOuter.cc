#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuter.hh"
#include "BDSSimpleComponent.hh"

#include "G4ThreeVector.hh"

BDSMagnetOuter::BDSMagnetOuter(G4VSolid*             containerSolid,
			       G4LogicalVolume*      containerLV,
			       BDSExtent             extent,
			       BDSGeometryComponent* magnetContainerIn,
			       G4ThreeVector         placementOffset,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn,
			       G4ThreeVector         inputFaceNormalIn,
			       G4ThreeVector         outputFaceNormalIn):
  BDSGeometryComponent(containerSolid, containerLV, extent, BDSExtent(),
		       placementOffset),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn,
			       G4ThreeVector         inputFaceNormalIn,
			       G4ThreeVector         outputFaceNormalIn):
  BDSGeometryComponent(*componentIn),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn)
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
  if (endPieceAfter && (endPieceAfter != endPieceBefore))
    {delete endPieceAfter; endPieceAfter = nullptr;}
  if (endPieceBefore)
    {delete endPieceBefore; endPieceBefore = nullptr;}
}

BDSMagnetOuter::~BDSMagnetOuter()
{
  ClearMagnetContainer();
  ClearEndPieces();
}
