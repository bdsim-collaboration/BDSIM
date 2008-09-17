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
#include "BDSMagFieldSQL.hh"

//using namespace std;

class BDSClassicalRK4;

class BDSGeometrySQL
{
public:
  BDSGeometrySQL(G4String DBfile, G4double markerlength);
  ~BDSGeometrySQL();

  void Construct(G4LogicalVolume *marker);

  // For List of uniform fields for volumes
  std::vector<G4ThreeVector> UniformField;
  std::vector<G4String> Fieldvol; 

  // For List of Quad/Sext/Oct Fields
  std::vector<G4double> QuadBgrad;
  std::vector<G4String> Quadvol; 
  std::vector<G4double> SextBgrad;
  std::vector<G4String> Sextvol;
  std::vector<G4double> OctBgrad;
  std::vector<G4String> Octvol;
  G4VPhysicalVolume* align_in_volume;
  G4VPhysicalVolume* align_out_volume;
  std::vector<G4LogicalVolume*> SensitiveComponents;

  std::vector<G4LogicalVolume*> VOL_LIST;
  G4bool HasFields;

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
  G4RotationMatrix* RotateComponent(G4double psi,
				    G4double phi,
				    G4double theta);
  void PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST);

  G4double itsMarkerLength;
  ifstream ifs;
  G4LogicalVolume* itsMarkerVol;
  std::vector<BDSMySQLTable*> itsSQLTable;
  BDSMagFieldSQL* itsMagField;
  BDSSamplerSD* SensDet;

protected:
};

#endif
