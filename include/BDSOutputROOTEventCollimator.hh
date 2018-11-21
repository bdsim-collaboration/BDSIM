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
#ifndef BDSOUTPUTROOTEVENTCOLLIMATOR_H
#define BDSOUTPUTROOTEVENTCOLLIMATOR_H

#ifndef __ROOTBUILD__
class BDSCollimatorHit;
class BDSTrajectoryPoint;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for each collimator per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventCollimator: public TObject
{
public:
  /// Default constructor for ROOT
  BDSOutputROOTEventCollimator();

  

  bool   interacted;
  bool   primaryStopped;
  
  double xIn;
  double yIn;
  double zIn;
  double SIn;
  double energyIn;
  double xOut;
  double yOut;
  double zOut;
  double SOut;

  double energyDeposited; ///< Sum of energy deposits including weights.

  double impactParameterX;
  double impactParameterY;

  // optional
  int n;
  std::vector<float> energy;
  std::vector<float> weight;
  std::vector<int>   partID;
  std::vector<float> z;           ///< Local z point for energy deposition.
  std::vector<float> stepLength;

  // further optional coordinates
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> T;       ///< Global time.
  std::vector<int>   turn;

  BDSOutputROOTEventCollimator();
  virtual ~BDSOutputROOTEventCollimator();
#ifndef __ROOTBUILD__
  void Fill();
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventCollimator, 1);
};


#endif
