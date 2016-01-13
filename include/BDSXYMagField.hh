#ifndef BDSXYMAGFIELD_H
#define BDSXYMAGFIELD_H

#include "BDSMagField.hh"

#include "globals.hh"
#include "G4ElectroMagneticField.hh"
#include "G4RotationMatrix.hh"

#include <vector>


struct XYFieldRecord {
  G4double x;
  G4double y;
  G4double Bx;
  G4double By;
  G4double Bz;
};

class BDSXYMagField : public BDSMagField
{
public:

  // mandatory members

  BDSXYMagField(G4String fname);
  
  ~BDSXYMagField();

  G4bool DoesFieldChangeEnergy() const;

  void GetFieldValue(const G4double Point[4],G4double *Bfield ) const;



  // aux members

  G4int AllocateMesh(G4int nX, G4int nY);

  G4int ReadFile(G4String fname);

  void Prepare(G4VPhysicalVolume *referenceVolume);

  void SetBx(G4int i,G4int j,G4double val);
  void SetBy(G4int i,G4int j,G4double val);
  void SetBz(G4int i,G4int j,G4double val);

  G4double GetBx(G4int i,G4int j);
  G4double GetBy(G4int i,G4int j);
  G4double GetBz(G4int i,G4int j);

private:
  
  G4double **Bx, **By, **Bz;

  std::vector<struct XYFieldRecord> itsFieldValues;

public:
  G4double xHalf, yHalf; // field mesh dimensions

  G4int nX, nY; // dimensions
  
  G4String itsFileName;
  
};


#endif
