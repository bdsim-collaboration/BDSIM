/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#ifndef TRKHybrid_h
#define TRKHybrid_h

#include "TRKStrategy.hh"

/**
 * @brief hybrid geant4 type tracking
 */
class TRKHybrid : public TRKStrategy {
public:

  TRKHybrid(int trackingSteps);
		     
  virtual ~TRKHybrid(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  virtual void Track(TRKDrift*      el, TRKBunch* bunch);
  virtual void Track(TRKDipole*     el, TRKBunch* bunch);
  virtual void Track(TRKSBend*       el, TRKBunch* bunch);
  virtual void Track(TRKRBend*       el, TRKBunch* bunch);
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch);
  virtual void Track(TRKSextupole*  el, TRKBunch* bunch);
  virtual void Track(TRKOctupole*   el, TRKBunch* bunch);
  virtual void Track(TRKSolenoid*   el, TRKBunch* bunch);
  ///@}

private:
  TRKHybrid() = delete; ///< not implemented
};


#endif
