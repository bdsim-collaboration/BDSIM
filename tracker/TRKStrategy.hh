/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef TRKStrategy_h
#define TRKStrategy_h 1

#include "TRKElement.hh"
#include "TRKParticle.hh"

class TRKBunch;
//class TRKDrift;
class TRKSBend;
class TRKRBend;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;
class TRKKicker;
class TRKSampler;
class TRKDipoleFringe;
class TRKTiltOffset;

/**
 * @brief virtual base class for a track strategy
 */
class TRKStrategy
{
public:
  TRKStrategy(int trackingStepsIn) : trackingSteps(trackingStepsIn) {}

  virtual ~TRKStrategy() {}

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  void SetReferenceParticle(BDSParticleDefinition* particle) { referenceParticle = particle;}
  /**
   * Tracks a particle for a given step length in an element
   * @param[in]  el    The TRKElement.
   * @param[in]  particle The particle.
   * @param[in]  step The step length.
   */
  ///@{
  //void         Track(TRKElement*    el, TRKBunch* bunch);
  virtual void Track(TRKDrift*      el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKSBend*      el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKRBend*      el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKDipoleFringe* el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKQuadrupole* el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKSextupole*  el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKOctupole*   el, TRKParticle& particle, double step)=0;
  virtual void Track(TRKKicker*     el, TRKParticle& particle, double step)=0;
  void         Track(TRKTiltOffset* el, TRKParticle& particle);
  ///@}

protected:
  /// number of tracking steps
  int trackingSteps;

  BDSParticleDefinition* referenceParticle = nullptr;

private:
  TRKStrategy() = delete; ///< not implemented
};


#endif
