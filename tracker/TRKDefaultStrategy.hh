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
#ifndef TRKDEFAULTSTRATEGY
#define TRKDEFAULTSTRATEGY

#include "TRKStrategy.hh"

class TRKOctupole;
class TRKQuadrupole;
class TRKRBend;
class TRKSBend;
class TRKSextupole;
class TRKKicker;
class TRKDipoleFringe;
typedef class TRKElement TRKDrift;

class TRKBunch;



class TRKDefaultStrategy : public TRKStrategy {
public:
  TRKDefaultStrategy() : TRKStrategy(0 /* Tracking steps */){};

  void Track(TRKDrift* el, TRKParticle& particle, double step) override;
  void Track(TRKSBend* el, TRKParticle& particle, double step) override;
  void Track(TRKRBend* el, TRKParticle& particle, double step) override;
  void Track(TRKDipoleFringe* el, TRKParticle& particle, double step) override;
  void Track(TRKQuadrupole* el, TRKParticle& particle, double step) override;
  void Track(TRKSextupole* el, TRKParticle& particle, double step) override;
  void Track(TRKOctupole* el, TRKParticle& particle, double step) override;
  void Track(TRKKicker* el, TRKParticle& particle, double step) override;

};

#endif
