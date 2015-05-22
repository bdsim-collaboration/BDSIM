#ifndef __BDS_GEOMETRY_HH_
#define __BDS_GEOMETRY_HH_

#include "BDSGeometryFormat.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4MagneticField.hh"
#include "BDSMagField.hh"
#include <vector>

class BDSGeometry{
public:
  BDSGeometry();
  ~BDSGeometry();
  BDSGeometry(const char* format, G4String file);
  BDSGeometry(G4String format, G4String file);
  BDSGeometry(BDSGeometryFormat* format, G4String file);

  BDSGeometryFormat* format() const;
  G4String file() const;

  virtual void Construct(G4LogicalVolume* marker)=0;

  void SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);
  std::vector<G4LogicalVolume*> GetSensitiveComponents();
  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes();
  std::vector<G4LogicalVolume*> GetGFlashComponents();

  G4VPhysicalVolume* align_out_volume();
  G4VPhysicalVolume* align_in_volume();

  void init();

  G4bool hasFields();
  G4int nPoleField();
  G4bool hasUniformField();

  G4double length();

  std::map<G4String, G4ThreeVector> uniformFieldVolField();
  std::map<G4String, G4double> quadVolBgrad();
  std::map<G4String, G4double> sextVolBgrad();
  std::map<G4String, G4double> octVolBgrad();

  G4MagneticField* field();

private:
  BDSGeometryFormat* _format;
  G4String _file;

protected:  
  std::vector<G4LogicalVolume*> _sensitiveComponents;
  std::vector<G4LogicalVolume*> _gFlashComponents;
  std::vector<G4VPhysicalVolume*> _multiplePhysicalVolumes;

  G4VPhysicalVolume* _align_in_volume;
  G4VPhysicalVolume* _align_out_volume;

  G4bool _hasFields;
  G4int _nPoleField;
  G4bool _hasUniformField;


  G4double _length; //overall length in z.

  std::map<G4String, G4ThreeVector> _uniformFieldVolField;
  std::map<G4String, G4double> _quadVolBgrad;
  std::map<G4String, G4double> _sextVolBgrad;
  std::map<G4String, G4double> _octVolBgrad;

  BDSMagField* _field;
};

inline void BDSGeometry::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{ _multiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4LogicalVolume*> BDSGeometry::GetSensitiveComponents()
{return _sensitiveComponents;}

inline  std::vector<G4VPhysicalVolume*> BDSGeometry::GetMultiplePhysicalVolumes()
{return _multiplePhysicalVolumes;}

inline  std::vector<G4LogicalVolume*> BDSGeometry::GetGFlashComponents()
{return _gFlashComponents;}

inline  G4VPhysicalVolume* BDSGeometry::align_in_volume(){
  return _align_in_volume;
}

inline  G4VPhysicalVolume* BDSGeometry::align_out_volume(){
  return _align_out_volume;
}

inline  G4bool BDSGeometry::hasFields(){
  return _hasFields;
}

inline  G4int BDSGeometry::nPoleField(){
  return _nPoleField;
}

inline  G4bool BDSGeometry::hasUniformField(){
  return _hasUniformField;
}

inline G4double BDSGeometry::length(){
  return _length;
}

inline std::map<G4String, G4ThreeVector> BDSGeometry::uniformFieldVolField() {
return _uniformFieldVolField;
}

inline std::map<G4String, G4double> BDSGeometry::quadVolBgrad(){
 return _quadVolBgrad;
}

inline std::map<G4String, G4double> BDSGeometry::sextVolBgrad(){
return _sextVolBgrad;
}

inline std::map<G4String, G4double> BDSGeometry::octVolBgrad(){
return _octVolBgrad;
}

inline  G4MagneticField* BDSGeometry::field(){
  return _field;
}

#endif
