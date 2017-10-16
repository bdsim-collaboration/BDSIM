#ifndef BDSBUNCHINTERFACE_H
#define BDSBUNCHINTERFACE_H 

#include "globals.hh"
#include "G4Transform3D.hh"

namespace CLHEP {
  class HepRandomEngine;
  class HepSymMatrix;
  class HepVector;
  class RandMultiGauss;
}

class BDSBeamline;
class BDSParticleDefinition;

namespace GMAD {
  class Beam;
}

/**
 * @brief The base class for bunch distribution generators.
 *
 * @author Stewart Boogert
 */

class BDSBunch
{ 
protected : 
  ///@{ Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double S0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double E0;
  G4double sigmaT; 
  G4double sigmaE;
  ///@}
  
  /// Whether to ignore z and use s and transform for curvilinear coordinates
  G4bool   useCurvilinear;

  /// Apply curvilinear transform. Otherwise apply transform for offset of the
  /// start of the beamline line. In the first case the beam line transform is picked
  /// up by definition.
  void ApplyTransform(G4double& x0, G4double& y0, G4double& z0,
		      G4double& xp, G4double& yp, G4double& zp) const;
  
  /// Transforms the coordinates from initial coordinates about 0,0,0 to
  /// those in a curvilinear system.  Here, z0 is treated as the intended
  /// S coordinate on input and is modifed to be the global z coordinate.
  void ApplyCurvilinearTransform(G4double& x0, G4double& y0, G4double& z0,
				 G4double& xp, G4double& yp, G4double& zp) const;

  /// Create multidimensional Gaussian random number generator
  /// for Twiss and Gauss, could be moved elsewhere
  /// can change sigma matrix to make non-definite
  CLHEP::RandMultiGauss* CreateMultiGauss(CLHEP::HepRandomEngine & anEngine,
					  const CLHEP::HepVector & mu,
					  CLHEP::HepSymMatrix & sigma);

  /// Calculate zp safely based on other components.
  G4double CalculateZp(G4double xp, G4double yp, G4double Zp0) const;

  /// Wether the bunch distribution can specify a particle that's different
  /// from the one used for the reference particle that created the beam line.
  /// Derived class must change explicitly.
  G4bool particleCanBeDifferent;

  /// Optional particle definition that can be used.
  BDSParticleDefinition* particleDefinition;

public : 
  BDSBunch();
  virtual ~BDSBunch();
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);

  /// Each derived class can override this default method of reference
  /// position. If S0 > 0 or derived class changes member bool 'curvilinear'
  /// z0 will be treated as S and the global z0 be calculated.
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

  inline G4bool ParticleCanBeDifferentFromBeam() const {return particleCanBeDifferent;}

  inline BDSParticleDefinition* ParticleDefinition() const {return particleDefinition;}

private:
  /// Transform that beam line starts with that will also be applied to coordinates.
  G4Transform3D beamlineTransform;

  /// Whether the transform is finite and should be used.
  G4bool        nonZeroTransform;
  
  /// A reference to the fully constructed beamline that's lazyily instantiated.
  mutable const BDSBeamline* beamline;
};

#endif
