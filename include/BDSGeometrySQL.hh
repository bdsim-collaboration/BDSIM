/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 13.04.2005
*/


#ifndef BDSGeometrySQL_h
#define BDSGeometrySQL_h 1

#include "globals.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"
#include "BDSMySQLTable.hh"
#include "G4VPhysicalVolume.hh"
//#include "BDSSamplerSD.hh"
#include <fstream>
#include <vector>
//#include "BDSMagFieldSQL.hh"
#include "G4Region.hh"
#include "BDSGeometry.hh"

class BDSGeometrySQL: public BDSGeometry
{
public:
  BDSGeometrySQL(G4String DBfile);
  ~BDSGeometrySQL();

  virtual void Construct(G4LogicalVolume *marker);

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

  std::map<G4String, G4ThreeVector> UniformFieldVolField;
  std::map<G4String, G4double> QuadVolBgrad;
  std::map<G4String, G4double> SextVolBgrad;
  std::map<G4String, G4double> OctVolBgrad;

  G4VPhysicalVolume* align_in_volume;
  G4VPhysicalVolume* align_out_volume;
  std::vector<G4LogicalVolume*> SensitiveComponents;
  std::vector<G4LogicalVolume*> itsGFlashComponents;
  std::vector<G4VPhysicalVolume*> itsMultiplePhysicalVolumes;

  std::vector<G4LogicalVolume*> VOL_LIST;
  G4bool HasFields;
  G4int nPoleField;
  G4bool HasUniformField;

  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes();
  std::vector<G4LogicalVolume*> GetGFlashComponents();

private:
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
  G4LogicalVolume* BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k);
  G4RotationMatrix* RotateComponent(G4double psi,
				    G4double phi,
				    G4double theta);
  G4RotationMatrix* rotateComponent;
  void PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST);

  std::ifstream ifs;
  G4LogicalVolume* itsMarkerVol;
  std::vector<BDSMySQLTable*> itsSQLTable;
  //  BDSMagFieldSQL* itsMagField;
  //  BDSSamplerSD* SensDet;

  void  SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);

  G4String _markerName; //The name given to the marker logical volume

protected:
};

inline void BDSGeometrySQL::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{ itsMultiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4VPhysicalVolume*> BDSGeometrySQL::GetMultiplePhysicalVolumes()
{return itsMultiplePhysicalVolumes;}

inline  std::vector<G4LogicalVolume*> BDSGeometrySQL::GetGFlashComponents()
{return itsGFlashComponents;}

#endif
