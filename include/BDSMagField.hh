#ifndef BDSMAGFIELD_H
#define BDSMAGFIELD_H

#include "G4MagneticField.hh"
#include "G4CachedMagneticField.hh"
#include "globals.hh"               // geant4 globals / types
#include "G4RotationMatrix.hh"

class G4VPhysicalVolume;

/**
 * @brief An abstract wrapper for G4MagneticField that provides the ability for a 
 * field map supplied in local cartesian coordinates to global cartesian
 * coordinates as required by Geant4 integrators. 
 * 
 * @author Lawrence Deacon <lawrence.deacon@ucl.ac.uk>
 */

class BDSMagField: public G4MagneticField
{
public:
  BDSMagField(G4String bmap        = "",
	      G4double bmapZOffset = "");  
  ~BDSMagField();

  /// Convert the local coordinates of the provided field mesh into
  /// global coordinates.
  virtual void Prepare(G4VPhysicalVolume* referenceVolume) = 0;

  void SetOriginRotation(G4RotationMatrix* rot);
  void SetOriginRotation(G4RotationMatrix rot);
  void SetOriginTranslation(G4ThreeVector trans);

public:
  virtual G4bool GetHasNPoleFields();
  virtual G4bool GetHasUniformField();
  virtual G4bool GetHasFieldMap();

  G4RotationMatrix Rotation() const;

  const G4String GetBFile();
  const G4String GetBFormat();
  
protected:
  void CheckPrepared() const;

private:
  /// Private default constructor to use provided one with defaults
  BDSMagField();
  
  void ParseBMapFormatAndFile();
  
  G4ThreeVector     translation;
  G4RotationMatrix* rotation;
  G4String          bmap;
  G4double          bmapZOffset;
  G4String          bFile;
  G4String          bFormat;
  G4bool            isPrepared;
  
};


#endif
