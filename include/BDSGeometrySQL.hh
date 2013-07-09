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
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "BDSMySQLTable.hh"
#include "G4VPhysicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "BDSSamplerSD.hh"
#include <fstream>
#include <vector>
#include <vector>
#include "BDSMagFieldSQL.hh"

class BDSClassicalRK4;

class BDSGeometrySQL
{
public:
  BDSGeometrySQL(G4String DBfile, G4double markerlength);
  ~BDSGeometrySQL();

  void Construct(G4LogicalVolume *marker);

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

  void BuildSQLObjects(G4String file);
  void BuildCone(BDSMySQLTable* aSQLTable);
  void BuildEllipticalCone(BDSMySQLTable* aSQLTable);
  void BuildPolyCone(BDSMySQLTable* aSQLTable);
  void BuildBox(BDSMySQLTable* aSQLTable);
  void BuildTrap(BDSMySQLTable* aSQLTable);
  void BuildTorus(BDSMySQLTable* aSQLTable);
  void BuildSampler(BDSMySQLTable* aSQLTable);
  void BuildTube(BDSMySQLTable* aSQLTable);
  void BuildEllipticalTube(BDSMySQLTable* aSQLTable);
  void BuildPCLTube(BDSMySQLTable* aSQLTable);
  G4RotationMatrix* RotateComponent(G4double psi,
				    G4double phi,
				    G4double theta);
  G4RotationMatrix* rotateComponent;
  void PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST);

  G4double itsMarkerLength;
  std::ifstream ifs;
  G4LogicalVolume* itsMarkerVol;
  std::vector<BDSMySQLTable*> itsSQLTable;
  BDSMagFieldSQL* itsMagField;
  BDSSamplerSD* SensDet;
  G4UserLimits* itsUserLimits;

void  SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);

protected:
};

inline void BDSGeometrySQL::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{ itsMultiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4VPhysicalVolume*> BDSGeometrySQL::GetMultiplePhysicalVolumes()
{return itsMultiplePhysicalVolumes;}

inline  std::vector<G4LogicalVolume*> BDSGeometrySQL::GetGFlashComponents()
{return itsGFlashComponents;}

#endif
