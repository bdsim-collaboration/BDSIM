#ifndef BDSSIMPLECOMPONENT_H
#define BDSSIMPLECOMPONENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSGeometryComponent;

/**
 * @brief A BDSAcceleratorComponent wrapper for BDSGeometryComponent.
 * 
 * This allows a BDSGeometryComponent to be placed in a beam line by
 * being a necessary BDSAcceleratorComponent, which gives the minimal 
 * information to BDSBeamline - name, length & angle.
 * 
 * This inherits the objects from the BDSGeometryComponent and therefore
 * effectively owns it without deleting it explicitly as a whole object.
 *
 * Face normal (unit) vectors are w.r.t. the incoming / outgoing reference 
 * trajectory and NOT the local geometry of the component. 
 *
 * @author Laurie Nevay
 */

class BDSSimpleComponent: public BDSAcceleratorComponent
{
public:
  /// Constructor that keeps the component as a daughter geometry component.
  /// Face normal (unit) vectors are w.r.t. the incoming / outgoing reference 
  /// trajectory and NOT the local geometry of the component.
  BDSSimpleComponent(G4String              name,
		     BDSGeometryComponent* componentIn,
		     G4double              arcLength,
		     G4double              angle = 0,
		     G4ThreeVector inputFaceNormal  = G4ThreeVector(0,0,-1),
		     G4ThreeVector outputFaceNormal = G4ThreeVector(0,0, 1));

  /// Alternate constructor that doesn't use a daughter geometry component.
  /// Face normal (unit) vectors are w.r.t. the incoming / outgoing reference 
  /// trajectory and NOT the local geometry of the component.
  BDSSimpleComponent(G4String              name,
		     G4double              arcLength,
		     G4double              angle,
		     G4VSolid*             containerSolidIn,
		     G4LogicalVolume*      containerLogicalVolumeIn,
		     G4ThreeVector         inputFaceNormal  = G4ThreeVector(0,0,-1),
		     G4ThreeVector         outputFaceNormal = G4ThreeVector(0,0, 1));

  /// Default destructor suffices as this calls base class which
  /// clears everything up.
  virtual ~BDSSimpleComponent(){;}

private:
  /// Private default constructor to force use of given one.
  BDSSimpleComponent();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSimpleComponent& operator=(const BDSSimpleComponent&);
  BDSSimpleComponent(BDSSimpleComponent&);
  /// @}

  /// Required implementation from base class.
  virtual void BuildContainerLogicalVolume(){;}
};

#endif
