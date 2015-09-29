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
  G4int _NVariables;
  G4double _VisRed; 
  G4double _VisGreen;
  G4double _VisBlue;
  G4double _VisAlpha;
  G4String _VisType;
  G4String _Material;
  G4String _TableName;
  G4String _Name;

  G4double _PosX;
  G4double _PosY;
  G4double _PosZ;
  G4double _RotPsi;
  G4double _RotTheta;
  G4double _RotPhi;
  G4double _K1,_K2,_K3,_K4;
  G4String _PARENTNAME;
  G4String _InheritStyle;
  G4String _Parameterisation;
  G4String _MagType;
  G4int _align_in;
  G4int _align_out;
  G4int _SetSensitive;
  G4int _PrecisionRegion;
  G4int _ApproximationRegion;
  G4double _FieldX, _FieldY, _FieldZ;

  G4double _lengthUserLimit;

  G4Region* _precisionRegionSQL;
  G4Region* _approximationRegionSQL;

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
