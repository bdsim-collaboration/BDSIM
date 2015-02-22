#ifndef BDSBEAMPIPE_H
#define BDSBEAMPIPE_H

#include "BDSGeometryComponent.hh"

#include "globals.hh"               // geant4 globals / types
#include "G4LogicalVolume.hh"

/**
 * @brief A holder class for all information required for a
 * piece of beampipe.  This does not implement the construction
 * of the beampipe. 
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipe: public BDSGeometryComponent
{
public:
  BDSBeamPipe(G4VSolid*                 containerSolidIn,
	      G4LogicalVolume*          containerLVIn,
	      std::pair<double, double> extentXIn,
	      std::pair<double, double> extentYIn,
	      std::pair<double, double> extentZIn,
	      G4LogicalVolume*          vacuumLVIn,
	      G4bool                    containerIsCircularIn = false,
	      G4double                  containerRadiusIn = 0.0
	      );
  ~BDSBeamPipe(); /// default destructor sufficient as G4 manages solids and LVs

  /// access the vacuum volume to set fields and limits
  G4LogicalVolume* GetVacuumLogicalVolume();
  /// flag to tell whether the parent volume needn't use a subtraction
  /// solid and can simply use a G4Tubs for example
  G4bool           ContainerIsCircular();
  /// if it is circular, we need the radius
  G4double         GetContainerRadius();
  
protected:
  G4LogicalVolume* vacuumLogicalVolume;
  G4bool           containerIsCircular;
  G4double         containerRadius;
};

inline G4LogicalVolume* BDSBeamPipe::GetVacuumLogicalVolume()
{return vacuumLogicalVolume;}

inline G4bool BDSBeamPipe::ContainerIsCircular()
{return containerIsCircular;}

inline G4double BDSBeamPipe::GetContainerRadius()
{return containerRadius;}

#endif
