#ifndef BDSBEAMLINEELEMENT_H
#define BDSBEAMLINEELEMENT_H

#include "BDSAcceleratorComponent.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <ostream>

/**
 * @brief A class that holds a fully constructed BDSAcceleratorComponent
 * as well as any information relevant to its position within the beamline.
 * For example, position information as curvilinear s position coordinate
 * as these are only defined with respect to the components position in the 
 * beamline / lattice.
 * 
 * Each instance of this class owns the position vectors and rotation matrices
 * as well as the accelerator component.
 */

class BDSBeamlineElement: public BDSGeometryComponent
{
public:
  BDSBeamlineElement(BDSAcceleratorComponent* component,
		     G4ThreeVector            positionStart,
		     G4ThreeVector            positionMiddle,
		     G4ThreeVector            positionEnd,
		     G4RotationMatrix*        rotationStart,
		     G4RotationMatrix*        rotationMiddle,
		     G4RotationMatrix*        rotationEnd,
		     G4double                 sPositionStart,
		     G4double                 sPositionMiddle,
		     G4double                 sPositionEnd);

  // Accessors @{
  inline BDSAcceleratorComponent* GetAcceleratorComponent() const;
  inline G4String                 GetName() const;
  inline G4ThreeVector            GetPositionStart() const;
  inline G4ThreeVector            GetPositionMiddle() const;
  inline G4ThreeVector            GetPositionEnd() const;
  inline G4RotationMatrix*        GetRotationStart() const;
  inline G4RotationMatrix*        GetRotationMiddle() const;
  inline G4RotationMatrix*        GetRotationEnd() const;
  inline G4double                 GetSPositionStart() const;
  inline G4double                 GetSPositionMiddle() const;
  inline G4double                 GetSPositionEnd() const;
  /** @} */

  /// output stream
  friend std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &element);
  
private:
  /// Private default constructor to force use of provided constructor
  BDSBeamlineElement();

  /// The accelerator component
  BDSAcceleratorComponent* component;

  /// The name of the object
  G4String name;

  /// Global coordinates for the start, middle and end of this beamline element
  G4ThreeVector     positionStart;
  G4ThreeVector     positionMiddle;
  G4ThreeVector     positionEnd;

  /// Global rotation matrices for the start, middle and end of this beamline element
  G4RotationMatrix* rotationStart;
  G4RotationMatrix* rotationMiddle;
  G4RotationMatrix* rotationEnd;

  /// S Positions for the start, middle and end of this beamline element
  G4double          sPositionStart;
  G4double          sPositionMiddle;
  G4double          sPositionEnd;

};

inline BDSAcceleratorComponent* BDSBeamlineElement::GetAcceleratorComponent() const
{return component;}

inline G4String                 BDSBeamlineElement::GetName() const
{return name;}

inline G4ThreeVector            BDSBeamlineElement::GetPositionStart() const
{return positionStart;}

inline G4ThreeVector            BDSBeamlineElement::GetPositionMiddle() const
{return positionMiddle;}

inline G4ThreeVector            BDSBeamlineElement::GetPositionEnd() const
{return positionEnd;}

inline G4RotationMatrix*        BDSBeamlineElement::GetRotationStart() const
{return rotationStart;}

inline G4RotationMatrix*        BDSBeamlineElement::GetRotationMiddle() const
{return rotationMiddle;}

inline G4RotationMatrix*        BDSBeamlineElement::GetRotationEnd() const
{return rotationEnd;}

inline G4double                 BDSBeamlineElement::GetSPositionStart() const
{return sPositionStart;}

inline G4double                 BDSBeamlineElement::GetSPositionMiddle() const
{return sPositionMiddle;}

inline G4double                 BDSBeamlineElement::GetSPositionEnd() const
{return sPositionEnd;}


#endif
