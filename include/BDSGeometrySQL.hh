#ifndef BDSGEOMETRYSQL_H
#define BDSGEOMETRYSQL_H

#include "BDSGeometry.hh"
#include "BDSMaterials.hh"
#include "BDSMySQLTable.hh"

#include "globals.hh" // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <fstream>
#include <vector>

class BDSGeometrySQL: public BDSGeometry
{
public:
  BDSGeometrySQL(G4String filePath);
  ~BDSGeometrySQL();

  virtual void Construct(G4LogicalVolume* containerLogicalVolume);

  // For List of uniform fields for volumes
  std::list<G4ThreeVector> UniformField;
  std::list<G4String> Fieldvol; 

  // For List of Quad/Sext/Oct Fields
  std::list<G4double> QuadBgrad;
  std::list<G4String> Quadvol; 
  std::list<G4double> SextBgrad;
  std::list<G4String> Sextvol;
  std::list<G4double> OctBgrad;
  std::list<G4String> Octvol;
  
  std::vector<G4LogicalVolume*> VOL_LIST;

private:
  /// Copy of the containing directory fo the main file. SQL main file
  /// typically only contains relative file paths to that main file.
  G4String containingDir;
  G4int NVariables;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4double VisAlpha;
  G4String VisType;
  G4String Material;
  G4String TableName;
  G4String Name;

  G4double PosX;
  G4double PosY;
  G4double PosZ;
  G4double RotPsi;
  G4double RotTheta;
  G4double RotPhi;
  G4double K1,K2,K3,K4;
  G4String PARENTNAME;
  G4String InheritStyle;
  G4String Parameterisation;
  G4String MagType;
  G4int align_in;
  G4int align_out;
  G4int SetSensitive;
  G4int PrecisionRegion;
  G4int ApproximationRegion;
  G4double FieldX, FieldY, FieldZ;

  G4double lengthUserLimit;

  G4Region* precisionRegionSQL;
  G4Region* approximationRegionSQL;

  void BuildSQLObjects(G4String file);
  void SetCommonParams(BDSMySQLTable*,G4int);
  void SetPlacementParams(BDSMySQLTable*,G4int);
  G4VisAttributes* VisAtt();
  G4UserLimits* UserLimits(G4double);
  void SetLogVolAtt(G4LogicalVolume*, G4double);
  void SetLogVolRegion(G4LogicalVolume*);
  G4LogicalVolume* BuildCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildEllipticalCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildPolyCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildBox(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTrap(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTorus(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildSampler(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTube(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildEllipticalTube(BDSMySQLTable* aSQLTable, G4int k);
  //G4LogicalVolume* BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k);
  G4RotationMatrix* RotateComponent(G4double psi,
				    G4double phi,
				    G4double theta);
  G4RotationMatrix* rotateComponent;
  void PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST);

  std::ifstream ifs;
  G4LogicalVolume* itsMarkerVol;
  std::vector<BDSMySQLTable*> itsSQLTable;
};


#endif
