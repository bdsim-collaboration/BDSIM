#ifndef BDSTRANSFORM3D_H
#define BDSTRANSFORM3D_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Transform in beam line that takes up no space.
 *
 * This applies a transformation to cumulative curvlinear
 * coordinates as the beam line is built allowing an aribtrary
 * rotation or displacement of the reference coordinates from
 * that point on. A tilt or offset should be used instead of 
 * two of these in conjunction (sandwiching) to offset a particular
 * BDSAcceleratorComponent.
 *
 * @author Laurie Nevay
 */

class BDSTransform3D: public BDSAcceleratorComponent
{
public:
  BDSTransform3D(G4String aName, G4double x,
		 G4double y, G4double z, G4double phi,
		 G4double theta, G4double psi);

  virtual ~BDSTransform3D();

  ///@{ Access the change in reference coordinates this component should induce
  inline G4double GetDX() const;
  inline G4double GetDY() const;
  inline G4double GetDZ() const;
  ///@}
  
  ///@{ Access the change in Euler angle this component should induce
  inline G4double GetDTheta() const;
  inline G4double GetDPsi()   const;
  inline G4double GetDPhi()   const;
  ///@}
  
private:
  /// Contractual function overload from virtual base class
  /// BDSAcceleratorComponent. In this case has null implementation
  /// as the transform only inherits BDSAcceleratorComponent so that
  /// is compatable with the beamline
  virtual void BuildContainerLogicalVolume(){;};

  G4double dx;
  G4double dy;
  G4double dz;
  G4double dTheta;
  G4double dPsi;
  G4double dPhi;
};

inline G4double BDSTransform3D::GetDX() const
{return dx;}

inline G4double BDSTransform3D::GetDY() const
{return dy;}

inline G4double BDSTransform3D::GetDZ() const
{return dz;}

inline G4double BDSTransform3D::GetDTheta() const
{return dTheta;}

inline G4double BDSTransform3D::GetDPsi() const
{return dPsi;}

inline G4double BDSTransform3D::GetDPhi() const
{return dPhi;}


#endif
