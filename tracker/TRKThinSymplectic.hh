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
#ifndef TRKThinSymplectic_h
#define TRKThinSymplectic_h

#include "TRKThin.hh"

/**
 * @brief Symplectic Thin tracking based on Sixtrack methods
 */
class TRKThinSymplectic : public TRKThin {
public:

  // todo discuss do we need trackingsteps for symplectic?
  TRKThinSymplectic(int trackingSteps);
		     
  virtual ~TRKThinSymplectic(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /// import all Track declarations from TRKThin, otherwise these are overwritten by specialised methods
  using TRKThin::Track;

  /**
   * Tracks a bunch through an element, replaces TRKThin method
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  virtual void Track(TRKDrift*  el, TRKBunch* bunch);
  virtual void Track(TRKDipole* el, TRKBunch* bunch);
  ///@}

  //  same as in TRKThin
  //  void Track(TRKQuadrupole* el, TRKBunch* bunch);
  //  void Track(TRKSextupole*  el, TRKBunch* bunch);
  //  void Track(TRKOctupole*   el, TRKBunch* bunch);

private:
  TRKThinSymplectic(); ///< not implemented
};

#endif
