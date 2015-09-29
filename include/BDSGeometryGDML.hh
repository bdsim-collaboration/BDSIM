#ifdef USE_GDML

#ifndef BDSGEOMETRYGDML_H
#define BDSGEOMETRYGDML_H

#include "BDSGeometry.hh"

#include "globals.hh" // geant4 types / globals

#include <vector>

class G4LogicalVolume;

class BDSGeometryGDML: public BDSGeometry
{
public:
  BDSGeometryGDML(G4String GMDLfile);
  ~BDSGeometryGDML();
  void Construct(G4LogicalVolume *marker);

  /// Access all logical volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;  
  /// Access all sensitive volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

private:
  G4String                      GDMLfile;
  G4LogicalVolume              *markerVol;
  G4LogicalVolume              *gdmlWorld;
  std::vector<G4LogicalVolume*> logicalVols; 
  std::vector<G4LogicalVolume*> sensitiveVols;
};
#endif

#endif
