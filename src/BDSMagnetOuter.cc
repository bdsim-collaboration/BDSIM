#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSMagnetOuter.hh"
#include "BDSSimpleComponent.hh"

#include "G4ThreeVector.hh"

BDSMagnetOuter::BDSMagnetOuter(G4VSolid*             containerSolidIn,
			       G4LogicalVolume*      containerLVIn,
			       BDSExtent             extentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       G4ThreeVector         placementOffsetIn,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn,
			       G4ThreeVector         inputFaceNormalIn,
			       G4ThreeVector         outputFaceNormalIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn, BDSExtent(),
		       placementOffsetIn),
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

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryExternal*  external,
			       BDSGeometryComponent* magnetContainerIn):
  BDSGeometryComponent(*(static_cast<BDSGeometryComponent*>(external))),
  magnetContainer(magnetContainerIn),
  endPieceBefore(nullptr),
  endPieceAfter(nullptr),
  inputFaceNormal(G4ThreeVector(0,0,-1)),
  outputFaceNormal(G4ThreeVector(0,0,1))
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
