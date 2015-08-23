#ifndef BDSMagFieldSQL_h
#define BDSMagFieldSQL_h 

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Navigator.hh"
#include "G4TouchableHistory.hh"
#include "G4TouchableHistoryHandle.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSMagField.hh"

class BDSMagFieldSQL: public BDSMagField, BDSAuxiliaryNavigator
{
public:  // with description

#if 0
  BDSMagFieldSQL(const G4String& FieldFile, G4double aMarkerLength,
		 std::list<G4String> Quadvol, std::list<G4double> aQuadBgrad,
		 std::list<G4String> Sextvol, std::list<G4double> aSextBgrad,
		 std::list<G4String> Octvol, std::list<G4double> aOctBgrad,
		 std::list<G4String> Fieldvol, std::list<G4ThreeVector> aUniformField);
#endif

  BDSMagFieldSQL(const G4String& aFieldFile,
		 G4double aMarkerLength,
		 std::map<G4String, G4double> aQuadVolBgrad,
		 std::map<G4String, G4double> aSextVolBgrad,
		 std::map<G4String, G4double> aOctVolBgrad,
		 std::map<G4String, G4ThreeVector> aUniformFieldVolField,
		 G4bool aHasNPoleFields, G4bool aHasUniformField);
  
  virtual ~BDSMagFieldSQL();
  // Constructor and destructor. No actions.


  void  GetFieldValue( const G4double Point[4],G4double *Bfield ) const;

  G4bool GetHasNPoleFields();
  G4bool GetHasUniformField();
  G4bool GetHasFieldMap();

  void Prepare(G4VPhysicalVolume *referenceVolume);

private:
  G4bool itsHasNPoleFields;
  G4bool itsHasUniformField;
  G4bool itsHasFieldMap;

  std::ofstream ofs;
  std::ifstream ifs;

  G4double itsMarkerLength;
  G4String FieldFile;

  // For Quad/Sext/Oct Fields
  std::list<G4double> itsQuadBgrad;
  std::list<G4String> itsQuadVol; 
  std::list<G4double> itsSextBgrad;
  std::list<G4String> itsSextVol; 
  std::list<G4double> itsOctBgrad;
  std::list<G4String> itsOctVol; 

  // For Uniform Fields
  std::list<G4ThreeVector> itsUniformField;
  std::list<G4String> itsFieldVol; 

  //Maps for fields
  std::map<G4String, G4ThreeVector> itsUniformFieldVolField;
  std::map<G4String, G4double> itsQuadVolBgrad;
  std::map<G4String, G4double> itsSextVolBgrad;
  std::map<G4String, G4double> itsOctVolBgrad;

  // For Solenoid FieldMaps
  std::vector<G4double> itsBz;
  std::vector<G4double> itsZ;
  std::vector<G4double> itsBr_over_r;
  std::vector<G4double> itsdBz_by_dz;
  double itsdz;
};

#endif
