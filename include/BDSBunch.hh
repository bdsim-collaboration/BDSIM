/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSBUNCH_H
#define BDSBUNCH_H

#include "globals.hh"
#include "G4Transform3D.hh"

#include <vector>

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
public:
  BDSBunch();
  virtual ~BDSBunch();

  /// Extract and set the relevant options from the beam definition.
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);

  /// Each derived class can override this default method of reference
  /// position. If S0 > 0 or derived class changes member bool 'curvilinear'
  /// z0 will be treated as S and the global z0 be calculated.
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

  /// Access whether the particle generated may be different from the design particle.
  inline G4bool ParticleCanBeDifferentFromBeam() const {return particleCanBeDifferent;}

  /// Access the beam particle definition.
  inline BDSParticleDefinition* ParticleDefinition() const {return particleDefinition;}
  
protected:
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
  
  /// Pregenerate all the particle coordinates and subtract the sample mean.
  void PreGenerateEvents();

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

  /// Wether the bunch distribution can specify a particle that's different
  /// from the one used for the reference particle that created the beam line.
  /// Derived class must change explicitly.
  G4bool particleCanBeDifferent;

  /// Optional particle definition that can be used.
  BDSParticleDefinition* particleDefinition;

  /// @{ Flags to ignore random number generator in case of no finite E or T.
  G4bool finiteSigmaE;
  G4bool finiteSigmaT;
  /// @}

  // Internal particle generation
  G4bool offsetSampleMean; ///< Whether to offset the sample mean.

  /// @{ Holder for pre-calcalculated coordinates.
  std::vector<G4double> x0_v, xp_v, y0_v, yp_v, z0_v, zp_v,E_v,t_v,weight_v;
  /// @}
  G4int iPartIteration; ///< Iterator for reading out pre-calculate coordinates
  
  /// Random number generators 
  CLHEP::RandMultiGauss* gaussMultiGen;
  
private:
  /// Transform that beam line starts with that will also be applied to coordinates.
  G4Transform3D beamlineTransform;

  /// Whether the transform is finite and should be used.
  G4bool        nonZeroTransform;
  
  /// A reference to the fully constructed beamline that's lazyily instantiated.
  mutable const BDSBeamline* beamline;
};

#endif
