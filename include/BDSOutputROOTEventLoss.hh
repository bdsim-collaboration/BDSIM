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
#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __ROOTBUILD__
class BDSEnergyCounterHit;
class BDSTrajectoryPoint;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for energy deposition hits per event.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventLoss: public TObject
{
public:
  int                       n = 0;   ///< Number of entries
  std::vector<float>        energy;  ///< Energy deposited in step
  std::vector<float>        S;       ///< Global curvilinear S coordinate
  std::vector<float>        weight;  ///< Weight associated with loss

  std::vector<int>          partID;  ///< ParticleID that create the deposit
  std::vector<int>          trackID; ///< TrackID that created the deposit
  std::vector<int>          parentID;///< ParentID that created the deposit
  //
  std::vector<int>          modelID; ///< Geometry model index
  std::vector<int>          turn;    ///< Turn number

  /// @{ Local coordinate
  std::vector<float>        x;
  std::vector<float>        y;
  std::vector<float>        z;
  /// @}
  
  //// @{ Global coordinates
  std::vector<float>        X;
  std::vector<float>        Y;
  std::vector<float>        Z;
  /// @}

  std::vector<float>        T; ///< global time (time since beginning of event)

  BDSOutputROOTEventLoss();
  BDSOutputROOTEventLoss(bool storeLinks, bool storeLocal, bool storeGobal,
                         bool storeTime);
  virtual ~BDSOutputROOTEventLoss();
#ifndef __ROOTBUILD__
  void Fill(const BDSTrajectoryPoint* hit);
  void Fill(const BDSEnergyCounterHit* hit);
#endif
  virtual void Flush();

  bool storeLinks  = false; // Store links between Eloss and model and trajectors
  bool storeLocal  = false; // Store local coordinates
  bool storeGlobal = false; // Store global coordinates
  bool storeTime   = false; // Store global time
  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
