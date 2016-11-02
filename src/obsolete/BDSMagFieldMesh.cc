#include "BDSDebug.hh"
#include "BDSMagFieldMesh.hh"

#include "globals.hh"  // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

BDSMagFieldMesh::BDSMagFieldMesh():
  translation(0,0,0),
  isPrepared(false),
  rotation(nullptr)
{;}

BDSMagFieldMesh::~BDSMagFieldMesh()
{delete rotation;}

void BDSMagFieldMesh::Prepare(G4VPhysicalVolume* /*referenceVolume*/)
{
  isPrepared = true;
}

void BDSMagFieldMesh::GetFieldValue( const G4double* /*point[4]*/,
				     G4double* bField) const
{
  bField[0] = 0;
  bField[1] = 0;
  bField[2] = 0;
  bField[3] = 0;
}

void BDSMagFieldMesh::SetOriginRotation(G4RotationMatrix* rot)
{rotation = rot;}

void BDSMagFieldMesh::SetOriginRotation(G4RotationMatrix rot)
{rotation = new G4RotationMatrix(rot);}

G4RotationMatrix BDSMagFieldMesh::Rotation() const
{return *rotation;}

void BDSMagFieldMesh::SetOriginTranslation(G4ThreeVector trans)
{translation = trans;}

G4bool BDSMagFieldMesh::HasNPoleFields()
{return false;}

G4bool BDSMagFieldMesh::HasUniformField()
{return false;}

G4bool BDSMagFieldMesh::HasFieldMap()
{return false;}

void BDSMagFieldMesh::CheckPrepared() const
{
  //Field must be prepared!
  if(!isPrepared)
    {
      G4String exceptionString = __METHOD_NAME__ + "- field has not been prepared. Rotation not set. Aborting.";
      G4Exception(exceptionString.c_str(), "-1", FatalErrorInArgument, "");   
    }
}
