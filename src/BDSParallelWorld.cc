#include "BDSParallelWorld.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4PSCellFlux.hh"

BDSParallelWorld::BDSParallelWorld(G4String worldName)
  :G4VUserParallelWorld(worldName)
{;}

BDSParallelWorld::~BDSParallelWorld(){
  delete _scoringMesh;
  delete _primitiveScorer;
}

void BDSParallelWorld::Construct() 
{
  G4VPhysicalVolume* ghostWorld = GetWorld();
  G4LogicalVolume* worldLogical = ghostWorld->GetLogicalVolume();

  //Scoring mesh
  _scoringMesh = new G4ScoringBox("scoringMesh");
  _scoringMesh->Construct(ghostWorld);
  G4int nSegments[]={100, 100, 100};
  G4Box* worldBox = (G4Box*)worldLogical->GetSolid();
  G4double x =  worldBox->GetXHalfLength();
  G4double y =  worldBox->GetYHalfLength();
  G4double z =  worldBox->GetZHalfLength();
  G4double size[]={x,y,z};
  
  _scoringMesh->SetNumberOfSegments(nSegments);
  _scoringMesh->SetSize(size);
  _primitiveScorer=(new G4PSCellFlux("cellFluxScorer"));
  _scoringMesh->SetPrimitiveScorer(_primitiveScorer);
  _scoringMesh->Activate();
}

G4ScoringBox* BDSParallelWorld::GetScoringMesh(){
  return _scoringMesh;
}
