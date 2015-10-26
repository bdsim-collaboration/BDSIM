#ifndef BDSSBENDFIELD_H
#define BDSSBENDFIELD_H

#include "globals.hh"
#include "G4MagneticField.hh"

#include "BDSAuxiliaryNavigator.hh"

class BDSSbendMagField: public G4MagneticField, public BDSAuxiliaryNavigator
{
public:
  /// A constant uniform field with value equal to 'field'. If angle is
  /// finite, the local radius of curvature is calculated and used by 
  BDSSbendMagField(G4ThreeVector  fieldIn,
		   const G4double aLength,
		   const G4double aAngle  = 0);
  
		       
  virtual ~BDSSbendMagField();

  /// The method Geant4 will use to access the field value
  virtual void  GetFieldValue(const G4double point[4],
			      G4double* bField) const;

  /// Access the local radius of curvature for the nominal momentum in this field.
  G4double GetLocalRadius() const;

private:
  /// The vector of the field. This constructed in local coordinates and the first time
  /// the field is queried, the auxiliary navigator (through inheritance) is used to
  /// transform the vector to global coordinates.
  G4ThreeVector field;

  /// A record of whether the field vector has been transformed to global coordinates.
  G4bool fieldTransformed;
  
  /// Radius of curvature.
  G4double localRadius;
};

inline G4double BDSSbendMagField::GetLocalRadius() const
{return localRadius;}

#endif
