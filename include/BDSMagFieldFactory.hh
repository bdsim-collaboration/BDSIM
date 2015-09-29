#ifndef BDSMAGFIELDFACTORY_H
#define BDSMAGFIELDFACTORY_H

#include "BDSBType.hh"
#include "BDSGeometry.hh"
#include "BDSMagFieldMesh.hh"

#include "globals.hh" // geant4 globals / types

class BDSGeometry;

class BDSMagFieldFactory
{
public:
  BDSMagFieldFactory();
  ~BDSMagFieldFactory();
  
  BDSMagFieldMesh* BuildMagneticField(G4String      formatAndFilePath,
				      G4ThreeVector offsetIn   = G4ThreeVector(0,0,0),
				      BDSGeometry*  geometryIn = nullptr);
 
private:
  
  G4ThreeVector offset;
  BDSBType      format;
  G4String      fileName;
  BDSGeometry*  geometry;
  G4double      cacheLength;

  /// Splits the G4String member variable formatAndName on the ":" character.
  /// Whatever is before is taken as the fromat string and whatever is after is
  /// taken as the filepath.
  void ParseFormatAndFilename();

  BDSMagFieldMesh* BuildMagField3D();
  BDSMagFieldMesh* BuildMagFieldXY();
  BDSMagFieldMesh* BuildMagFieldLCDD();
  BDSMagFieldMesh* BuildMagFieldSQL();
};
#endif
