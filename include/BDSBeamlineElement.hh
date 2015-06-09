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
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
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
		     G4ThreeVector            referencePositionStart,
		     G4ThreeVector            referencePositionMiddle,
		     G4ThreeVector            referencePositionEnd,
		     G4RotationMatrix*        referenceRotationStart,
		     G4RotationMatrix*        referenceRotationMiddle,
		     G4RotationMatrix*        referenceRotationEnd,
		     G4double                 sPositionStart,
		     G4double                 sPositionMiddle,
		     G4double                 sPositionEnd,
		     G4ThreeVector            xAxisReferenceStart,
		     G4ThreeVector            yAxisReferenceStart,
		     G4ThreeVector            zAxisReferenceStart,
		     G4ThreeVector            xAxisReferenceMiddle,
		     G4ThreeVector            yAxisReferenceMiddle,
		     G4ThreeVector            zAxisReferenceMiddle,
		     G4ThreeVector            xAxisReferenceEnd,
		     G4ThreeVector            yAxisReferenceEnd,
		     G4ThreeVector            zAxisReferenceEnd);

  ~BDSBeamlineElement();
  
  ///@{ Accessor
  inline BDSAcceleratorComponent* GetAcceleratorComponent()    const;
  inline G4String                 GetName()                    const;
  inline G4ThreeVector            GetPositionStart()           const;
  inline G4ThreeVector            GetPositionMiddle()          const;
  inline G4ThreeVector            GetPositionEnd()             const;
  inline G4RotationMatrix*        GetRotationStart()           const;
  inline G4RotationMatrix*        GetRotationMiddle()          const;
  inline G4RotationMatrix*        GetRotationEnd()             const;
  inline G4ThreeVector            GetReferencePositionStart()  const;
  inline G4ThreeVector            GetReferencePositionMiddle() const;
  inline G4ThreeVector            GetReferencePositionEnd()    const;
  inline G4RotationMatrix*        GetReferenceRotationStart()  const;
  inline G4RotationMatrix*        GetReferenceRotationMiddle() const;
  inline G4RotationMatrix*        GetReferenceRotationEnd()    const;
  inline G4double                 GetSPositionStart()          const;
  inline G4double                 GetSPositionMiddle()         const;
  inline G4double                 GetSPositionEnd()            const;
  inline G4ThreeVector            GetXAxisReferenceStart()     const;
  inline G4ThreeVector            GetYAxisReferenceStart()     const;
  inline G4ThreeVector            GetZAxisReferenceStart()     const;
  inline G4ThreeVector            GetXAxisReferenceMiddle()    const;
  inline G4ThreeVector            GetYAxisReferenceMiddle()    const;
  inline G4ThreeVector            GetZAxisReferenceMiddle()    const;
  inline G4ThreeVector            GetXAxisReferenceEnd()       const;
  inline G4ThreeVector            GetYAxisReferenceEnd()       const;
  inline G4ThreeVector            GetZAxisReferenceEnd()       const;
  ///@}

  ///@{ Reassign the end variable as required when applying a transform
  inline void SetReferencePositionEnd(G4ThreeVector     newReferencePositionEnd);
  inline void SetReferenceRotationEnd(G4RotationMatrix* newReferenceRotatonEnd);
  inline void SetXAxisReferenceEnd   (G4ThreeVector     newXAxisReferenceEnd);
  inline void SetYAxisReferenceEnd   (G4ThreeVector     newYAxisReferenceEnd);
  inline void SetZAxisReferenceEnd   (G4ThreeVector     newZAxisReferenceEnd);
  ///@}

  /// output stream
  friend std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &element);
  
private:
  /// Private default constructor to force use of provided constructor
  BDSBeamlineElement();

  /// The accelerator component
  BDSAcceleratorComponent* component;
  
  ///@{ Global coordinates for the start, middle and end of this beamline element
  G4ThreeVector     positionStart;
  G4ThreeVector     positionMiddle;
  G4ThreeVector     positionEnd;
  ///@}
  
  ///@{ Global rotation matrices for the start, middle and end of this beamline element
  G4RotationMatrix* rotationStart;
  G4RotationMatrix* rotationMiddle;
  G4RotationMatrix* rotationEnd;
  ///@}

  ///@{ Global coordinates for the start, middle and end of this beamline element
  /// along the reference trajectory without any component offsets / displacements
  G4ThreeVector     referencePositionStart;
  G4ThreeVector     referencePositionMiddle;
  G4ThreeVector     referencePositionEnd;
  ///@}
  
  ///@{ Global rotation matrices for the start, middle and end of this beamline element
  /// along the reference trajectory without any tilt or rotation from the component
  G4RotationMatrix* referenceRotationStart;
  G4RotationMatrix* referenceRotationMiddle;
  G4RotationMatrix* referenceRotationEnd;
  ///@}

  ///@{ S Positions for the start, middle and end of this beamline element
  G4double          sPositionStart;
  G4double          sPositionMiddle;
  G4double          sPositionEnd;
  ///@}

  ///@{ Vector representing the local (X,Y,Z) axis after cumulative rotation and translation
  ///   along the line. One for each of the start, middle and end of each component. These are
  ///   required to apply the rotations about.
  G4ThreeVector     xAxisReferenceStart;
  G4ThreeVector     yAxisReferenceStart;
  G4ThreeVector     zAxisReferenceStart;
  G4ThreeVector     xAxisReferenceMiddle;
  G4ThreeVector     yAxisReferenceMiddle;
  G4ThreeVector     zAxisReferenceMiddle;
  G4ThreeVector     xAxisReferenceEnd;
  G4ThreeVector     yAxisReferenceEnd;
  G4ThreeVector     zAxisReferenceEnd;
  ///@}
};

inline BDSAcceleratorComponent* BDSBeamlineElement::GetAcceleratorComponent() const
{return component;}

inline G4String                 BDSBeamlineElement::GetName() const
{return component->GetName();}

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

inline G4ThreeVector            BDSBeamlineElement::GetReferencePositionStart() const
{return referencePositionStart;}

inline G4ThreeVector            BDSBeamlineElement::GetReferencePositionMiddle() const
{return referencePositionMiddle;}

inline G4ThreeVector            BDSBeamlineElement::GetReferencePositionEnd() const
{return referencePositionEnd;}

inline G4RotationMatrix*        BDSBeamlineElement::GetReferenceRotationStart() const
{return referenceRotationStart;}

inline G4RotationMatrix*        BDSBeamlineElement::GetReferenceRotationMiddle() const
{return referenceRotationMiddle;}

inline G4RotationMatrix*        BDSBeamlineElement::GetReferenceRotationEnd() const
{return referenceRotationEnd;}

inline G4double                 BDSBeamlineElement::GetSPositionStart() const
{return sPositionStart;}

inline G4double                 BDSBeamlineElement::GetSPositionMiddle() const
{return sPositionMiddle;}

inline G4double                 BDSBeamlineElement::GetSPositionEnd() const
{return sPositionEnd;}

inline G4ThreeVector            BDSBeamlineElement::GetXAxisReferenceStart() const
{return xAxisReferenceStart;}

inline G4ThreeVector            BDSBeamlineElement::GetYAxisReferenceStart() const
{return yAxisReferenceStart;}

inline G4ThreeVector            BDSBeamlineElement::GetZAxisReferenceStart() const
{return zAxisReferenceStart;}

inline G4ThreeVector            BDSBeamlineElement::GetXAxisReferenceMiddle() const
{return xAxisReferenceMiddle;}

inline G4ThreeVector            BDSBeamlineElement::GetYAxisReferenceMiddle() const
{return yAxisReferenceMiddle;}

inline G4ThreeVector            BDSBeamlineElement::GetZAxisReferenceMiddle() const
{return zAxisReferenceMiddle;}

inline G4ThreeVector            BDSBeamlineElement::GetXAxisReferenceEnd() const
{return xAxisReferenceEnd;}

inline G4ThreeVector            BDSBeamlineElement::GetYAxisReferenceEnd() const
{return yAxisReferenceEnd;}

inline G4ThreeVector            BDSBeamlineElement::GetZAxisReferenceEnd() const
{return zAxisReferenceEnd;}


#endif
